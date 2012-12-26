/*
 * Telefónica Digital - Product Development and Innovation
 *
 * THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND,
 * EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
 *
 * Copyright (c) Telefónica Investigación y Desarrollo S.A.U.
 * All rights reserved.
 */
#include "samson/stream/BlockLookupList.h"  // Own interface

#include "au/mutex/TokenTaker.h"

#include "engine/DiskManager.h"                     // notification_disk_operation_request_response
#include "engine/DiskOperation.h"                   // engine::DiskOperation
#include "engine/Engine.h"                          // engine::Engine
#include "engine/MemoryManager.h"                   // engine::MemoryManager
#include "engine/Notification.h"                    // engine::Notification
#include "engine/ProcessManager.h"

#include "logMsg/logMsg.h"                          // LOG_SW

#include "samson/common/KVHeader.h"
#include "samson/common/Logs.h"
#include "samson/common/MessagesOperations.h"
#include "samson/common/SamsonSetup.h"              // samson::SamsonSetup
#include "samson/module/KVFormat.h"
#include "samson/module/ModulesManager.h"           // ModulesManager
#include "samson/stream/Block.h"
#include "samson/stream/BlockList.h"                // BlockList

#include "samson/stream/BlockManager.h"             // BlockManager

namespace samson {
namespace stream {
BlockLookupList::BlockLookupList(Block *block) {
  // Get id of the block
  block_id_ = block->block_id();

  // Data pointer
  char *data = block->buffer()->data();

  // Vector of KVInfo per hash-group
  KVInfo *kvInfoV = createKVInfoVector(data, &error);

  if (!kvInfoV) {
    head = NULL;
    hashInfo = NULL;
    return;
  }

  LOG_V(logs.rest, ("Creating lookup list"));

  // Store format locally
  kvFormat = block->getKVFormat();

  // semTake();
  head = reinterpret_cast<BlockLookupRecord *>(calloc(block->header.info.kvs, sizeof(BlockLookupRecord)));
  size = block->header.info.kvs;

  if (head == NULL) {
    LM_X(1,
         ("Error allocating lookupList.head of %d bytes", block->header.info.kvs *
          sizeof(BlockLookupRecord)));
  }
  hashInfo = reinterpret_cast<BlockHashLookupRecord *>(calloc(KVFILE_NUM_HASHGROUPS, sizeof(BlockHashLookupRecord)));
  if (hashInfo == NULL) {
    LM_X(1,
         ("Error allocating lookupList.hashInfo of %d bytes", KVFILE_NUM_HASHGROUPS *
          sizeof(BlockHashLookupRecord)));
  }
  LOG_V(logs.rest,
        ("Created a lookup list for %d records",
         block->header.info.kvs));

  unsigned int hashIx;
  unsigned int kvIx;
  unsigned int offset = 0;
  unsigned int noOfKvs = 0;
  char *kvsStart = data + sizeof(KVHeader);
  Data *keyData = au::Singleton<ModulesManager>::shared()->GetData(kvFormat.keyFormat);
  Data *valueData = au::Singleton<ModulesManager>::shared()->GetData(kvFormat.valueFormat);
  DataInstance *keyDataInstance = reinterpret_cast<DataInstance *>(keyData->getInstance());
  DataInstance *valueDataInstance = reinterpret_cast<DataInstance *>(valueData->getInstance());
  int maxEntries = 0;
  int maxEntryHashGroup = -1;

  for (hashIx = 0; hashIx < KVFILE_NUM_HASHGROUPS; hashIx++) {
    if (kvInfoV[hashIx].kvs != 0) {
      int entries = kvInfoV[hashIx].kvs;

      if (entries > maxEntries) {
        maxEntries = entries;
        maxEntryHashGroup = hashIx;
      }

      LOG_D(logs.rest,
            ("setting hashInfo[%d]: %d-%d (%d entries in hashgroup - max entries is %d)"
             , hashIx
             , noOfKvs
             , noOfKvs + kvInfoV[hashIx].kvs
             , entries, maxEntries));
    }

    hashInfo[hashIx].startIx = noOfKvs;
    hashInfo[hashIx].endIx = noOfKvs + kvInfoV[hashIx].kvs;

    for (kvIx = 0; kvIx < kvInfoV[hashIx].kvs; kvIx++) {
      int keySize = keyDataInstance->parse(kvsStart + offset);
      int valueSize = valueDataInstance->parse(kvsStart + offset + keySize);

      head[noOfKvs].keyP = kvsStart + offset;
      head[noOfKvs].keyS = keySize;

      offset += (keySize + valueSize);
      ++noOfKvs;
    }
  }
  LOG_V(logs.rest, ("Hash Group %d has %d entries", maxEntryHashGroup, maxEntries));

  // int wordIx = 0;
  // for (size_t ix = lookupList.hashInfo[maxEntryHashGroup].startIx; ix <= lookupList.hashInfo[maxEntryHashGroup].endIx; ix++)
  // {
  //     samson::system::String string;
  //     string->parse(lookupList.head[ix].keyP);
  //
  //     LM_T(LmtHash, ("HG %d, Word %02d: '%s'", maxEntryHashGroup, wordIx, string->value.c_str()));
  //     ++wordIx;
  // }

  LOG_V(logs.rest, ("lookup list created"));
  // semGive();


  if (kvInfoV) {
    free(kvInfoV);
  }
}

BlockLookupList::~BlockLookupList() {
  if (head != NULL) {
    free(head);
  }
  if (hashInfo != NULL) {
    free(hashInfo);
  }
}

int compare_binary_keys(char *key, size_t key_size, char *key2, size_t key_size2) {
  if (key_size < key_size2) {
    return -1;
  }

  if (key_size > key_size2) {
    return 1;
  }

  for (size_t i = 0; i < key_size; i++) {
    if (key[i] < key2[i]) {
      return -1;
    }

    if (key[i] > key2[i]) {
      return 1;
    }
  }
  return 0;
}

void BlockLookupList::lookup(const char *key, au::SharedPointer<au::network::RESTServiceCommand> command) {
  int hashGroup;
  int keySize;
  int testKeySize;
  char keyName[1024];
  Data *keyData = au::Singleton<ModulesManager>::shared()->GetData(kvFormat.keyFormat);
  DataInstance *keyDataInstance = reinterpret_cast<DataInstance *>(keyData->getInstance());
  int compare;

  // Set instance from provided string ( detect possible error in this convertion )
  std::string error_message;

  if (!keyDataInstance->setContentFromString(key, error_message)) {
    command->AppendFormatedError(au::str("Error converting %s into a valid %s (%s)"
                                         , key, kvFormat.keyFormat.c_str(), error_message.c_str()));
    return;
  }

  keySize = keyDataInstance->serialize(keyName);
  hashGroup = keyDataInstance->hash(KVFILE_NUM_HASHGROUPS);
  LOG_V(logs.rest,
        ("looking up key '%s'(keyDataInstance:name:%s, val:%s) in hashgroup:%d", key,
         keyDataInstance->getName().c_str(),
         keyDataInstance->str().c_str(), hashGroup));

  int startIx = hashInfo[hashGroup].startIx;
  int endIx = hashInfo[hashGroup].endIx;
  int testIx = (endIx - startIx) / 2 + startIx;

  while (true) {
    LOG_D(logs.rest, ("looking up key '%s' - comparing with ix %d (from ix %d to %d)"
                      , key, testIx, startIx, endIx));

    // We compare key and values only looking at the binary representation
    compare = compare_binary_keys(keyName, keySize, head[testIx].keyP, head[testIx].keyS);

    if (compare == 0) {
      testKeySize = keyDataInstance->parse(head[testIx].keyP);

      Data *valueData = au::Singleton<ModulesManager>::shared()->GetData(kvFormat.valueFormat);
      char *valueP = head[testIx].keyP + testKeySize;
      DataInstance *valueDataInstance = reinterpret_cast<DataInstance *>(valueData->getInstance());

      valueDataInstance->parse(valueP);

      LOG_V(logs.rest, ("Match key '%s' - at ix %d (testKeySize:%d, name:%s, path:%s)"
                        , key
                        , testIx
                        , testKeySize
                        , valueDataInstance->getName().c_str()
                        , valueDataInstance->getType()));

      // Output key and Value
      command->AppendFormatedElement("key", keyDataInstance->strFormatted(command->format()));
      command->AppendFormatedElement("value", valueDataInstance->strFormatted(command->format()));
      return;
    }

    if (compare < 0) {
      // keyName < testKey => Go to the left - to 'smaller' key names
      endIx = testIx - 1;
    } else {
      startIx = testIx + 1;
    }
    testIx = (endIx - startIx) / 2 + startIx;

    if (startIx > endIx) {
      LOG_V(logs.rest, ("Key '%s' not found", key));
      command->AppendFormatedError("Key not found");
      return;
    }
  }
}
}
}

