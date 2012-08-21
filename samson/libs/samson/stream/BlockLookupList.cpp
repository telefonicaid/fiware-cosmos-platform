#include "logMsg/logMsg.h"                          // LM_W


#include "au/mutex/TokenTaker.h"

#include "engine/DiskManager.h"                     // notification_disk_operation_request_response
#include "engine/DiskOperation.h"                   // engine::DiskOperation
#include "engine/Engine.h"                          // engine::Engine
#include "engine/MemoryManager.h"                   // engine::MemoryManager
#include "engine/Notification.h"                    // engine::Notification
#include "engine/ProcessManager.h"

#include "samson/common/MessagesOperations.h"
// MemoryBlocks
#include "samson/common/KVHeader.h"
#include "samson/common/SamsonSetup.h"              // samson::SamsonSetup

#include "samson/module/KVFormat.h"
#include "samson/module/ModulesManager.h"           // ModulesManager

#include "BlockList.h"                              // BlockList
#include "BlockManager.h"                           // BlockManager

#include "Block.h"

#include "BlockLookupList.h"  // Own interface

namespace samson { namespace stream {
                   BlockLookupList::BlockLookupList(Block *block) {
                     char *data = block->buffer()->getData();

                     // Vector of KVInfo per hash-group
                     KVInfo *kvInfoV  = createKVInfoVector(data, &error);

                     if (!kvInfoV) {
                       head = NULL;
                       hashInfo = NULL;
                       return;
                     }

                     LM_T(LmtRest, ("Creating lookup list"));

                     // Store format locally
                     kvFormat = block->getKVFormat();

                     // semTake();
                     head = (BlockLookupRecord *)calloc(block->header.info.kvs, sizeof(BlockLookupRecord));
                     size = block->header.info.kvs;

                     if (head == NULL)
                       LM_X(1, ("Error allocating lookupList.head of %d bytes", block->header.info.kvs * sizeof(BlockLookupRecord)));
                     hashInfo = (BlockHashLookupRecord *)calloc(KVFILE_NUM_HASHGROUPS, sizeof(BlockHashLookupRecord));
                     if (hashInfo == NULL)
                       LM_X(1, ("Error allocating lookupList.hashInfo of %d bytes", KVFILE_NUM_HASHGROUPS * sizeof(BlockHashLookupRecord)));
                     LM_T(LmtRest, ("Created a lookup list for %d records", block->header.info.kvs));

                     unsigned int hashIx;
                     unsigned int kvIx;
                     unsigned int offset              = 0;
                     unsigned int noOfKvs             = 0;
                     char *kvsStart            = data + sizeof(KVHeader);
                     Data *keyData             = ModulesManager::shared()->getData(kvFormat.keyFormat);
                     Data *valueData           = ModulesManager::shared()->getData(kvFormat.valueFormat);
                     DataInstance *keyDataInstance     = (DataInstance *)keyData->getInstance();
                     DataInstance *valueDataInstance   = (DataInstance *)valueData->getInstance();
                     int maxEntries          = 0;
                     int maxEntryHashGroup   = -1;

                     for (hashIx = 0; hashIx < KVFILE_NUM_HASHGROUPS; hashIx++) {
                       if (kvInfoV[hashIx].kvs != 0) {
                         int entries = kvInfoV[hashIx].kvs;

                         if (entries > maxEntries) {
                           maxEntries        = entries;
                           maxEntryHashGroup = hashIx;
                         }

                         LM_T(LmtRest,
                              ("setting hashInfo[%d]: %d-%d (%d entries in hashgroup - max entries is %d)", hashIx, noOfKvs, noOfKvs +
                               kvInfoV[hashIx].kvs,
                               entries, maxEntries));
                       }

                       hashInfo[hashIx].startIx = noOfKvs;
                       hashInfo[hashIx].endIx   = noOfKvs + kvInfoV[hashIx].kvs;

                       for (kvIx = 0; kvIx < kvInfoV[hashIx].kvs; kvIx++) {
                         int keySize   = keyDataInstance->parse(kvsStart + offset);
                         int valueSize = valueDataInstance->parse(kvsStart + offset + keySize);

                         head[noOfKvs].keyP = kvsStart + offset;
                         head[noOfKvs].keyS = keySize;

                         offset += (keySize + valueSize);
                         ++noOfKvs;
                       }
                     }
                     LM_T(LmtRest, ("Hash Group %d has %d entries", maxEntryHashGroup, maxEntries));

                     // int wordIx = 0;
                     // for (size_t ix = lookupList.hashInfo[maxEntryHashGroup].startIx; ix <= lookupList.hashInfo[maxEntryHashGroup].endIx; ix++)
                     // {
                     //     samson::system::String string;
                     //     string->parse(lookupList.head[ix].keyP);
                     //
                     //     LM_T(LmtHash, ("HG %d, Word %02d: '%s'", maxEntryHashGroup, wordIx, string->value.c_str()));
                     //     ++wordIx;
                     // }

                     LM_T(LmtRest, ("lookup list created"));
                     // semGive();


                     if (kvInfoV)
                       free(kvInfoV);
                   }

                   BlockLookupList::~BlockLookupList() {
                     if (head != NULL)
                       free(head); if (hashInfo != NULL)
                       free(hashInfo);
                   }

                   int compare_binary_keys(char *key, size_t key_size, char *key2, size_t key_size2) {
                     if (key_size < key_size2)
                       return -1;

                     if (key_size > key_size2)
                       return 1;

                     for (size_t i = 0; i < key_size; i++) {
                       if (key[i] < key2[i])
                         return -1;

                       if (key[i] > key2[i])
                         return 1;
                     }
                     return 0;
                   }

                   std::string BlockLookupList::lookup(const char *key, std::string outputFormat) {
                     int hashGroup;
                     int keySize;
                     int testKeySize;
                     char keyName[1024];
                     Data *keyData             = ModulesManager::shared()->getData(kvFormat.keyFormat);
                     DataInstance *keyDataInstance     = (DataInstance *)keyData->getInstance();
                     int compare;


                     keyDataInstance->setFromString(key);

                     keySize      = keyDataInstance->serialize(keyName);
                     hashGroup    = keyDataInstance->hash(KVFILE_NUM_HASHGROUPS);
                     LM_T(LmtRest,
                          ("looking up key '%s'(keyDataInstance:name:%s, val:%s) in hashgroup:%d", key, keyDataInstance->getName().c_str(),
                           keyDataInstance->str().c_str(), hashGroup));

                     int startIx  = hashInfo[hashGroup].startIx;
                     int endIx    = hashInfo[hashGroup].endIx;
                     int testIx   = (endIx - startIx) / 2 + startIx;

                     while (true) {
                       LM_T(LmtRest, ("looking up key '%s' - comparing with ix %d (from ix %d to %d)", key, testIx, startIx, endIx));
                       // Andreu: We compare key and values only looking at the binary representation
                       // compare = keyDataInstance->serial_compare(keyName, head[testIx].keyP);
                       compare = compare_binary_keys(keyName, keySize, head[testIx].keyP, head[testIx].keyS);

                       if (compare == 0) {
                         testKeySize = keyDataInstance->parse(head[testIx].keyP);

                         Data *valueData          = ModulesManager::shared()->getData(kvFormat.valueFormat);
                         char *valueP             = (char *)((size_t)head[testIx].keyP + testKeySize);
                         DataInstance *valueDataInstance  = (DataInstance *)valueData->getInstance();

                         valueDataInstance->parse(valueP);

                         LM_T(LmtRest,
                              ("Match key '%s' - at ix %d (testKeySize:%d, name:%s, path:%s)", key, testIx, testKeySize,
                               valueDataInstance->getName().c_str(),
                               valueDataInstance->getType()));

                         if (outputFormat == "xml") {
                           std::ostringstream output;
                           output << "<key>\n";
                           output << keyDataInstance->strXML();
                           output << "</key>\n";
                           output << "<value>\n";
                           output << valueDataInstance->strXML();
                           output << "</value>\n";
                           return output.str();
                         }
                         if (outputFormat == "html") {
                           std::ostringstream output;
                           output << "<h1>key</h1>\n";
                           output << keyDataInstance->strHTML(2);
                           output << "<h1>value</h1>\n";
                           output << valueDataInstance->strHTML(2);
                           return output.str();
                         }
                         if (outputFormat == "thtml") {
                           std::ostringstream output;

                           output << keyDataInstance->strHTMLTable("key");

                           output << valueDataInstance->strHTMLTable("value");

                           return output.str();
                         } else {
                           std::ostringstream output;
                           output << "{ \"key\" : ";
                           output << keyDataInstance->strJSON();
                           output << ",\"value\" : ";
                           output << valueDataInstance->strJSON();
                           output << " } ";
                           return output.str();
                         }
                       }

                       if (compare < 0) // keyName < testKey => Go to the left - to 'smaller' key names
                         endIx = testIx - 1; else
                         startIx = testIx + 1; testIx = (endIx - startIx) / 2 + startIx;

                       if (startIx > endIx) {  // Not found
                         if (outputFormat == "xml")
                           return au::xml_simple("error", au::str("Key %s not found", key)); else
                           return std::string("  \"error\" : \"") + au::str("Key %s not found\"\r\n", key);
                       }
                     }
                   }
                   } }


