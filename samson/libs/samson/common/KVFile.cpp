
#include "au/string.h"
#include "logMsg/logMsg.h"
#include <stdlib.h>  // malloc

#include "samson/module/Data.h"
#include "samson/module/ModulesManager.h"


#include "KVFile.h"  // Own interface

namespace samson {
au::SharedPointer<KVFile> KVFile::create(engine::BufferPointer buffer, au::ErrorManager& error) {
  // Candidate instance
  au::SharedPointer<KVFile> kv_file(new KVFile());
  kv_file->buffer_ = buffer;

  if (buffer == NULL) {
    error.set("NULL buffer provided");
    return au::SharedPointer<KVFile>(NULL);
  }

  if (buffer->size() < sizeof(KVHeader)) {
    error.set(au::str("Incorrect buffer size (%lu) < header size ", buffer->size()));
    return au::SharedPointer<KVFile>(NULL);
  }

  // Copy & check header of this packet
  memcpy(&kv_file->header_, buffer->data(), sizeof(KVHeader));
  if (!kv_file->header_.check()) {
    error.set("KVHeader error: wrong magic number");
    return au::SharedPointer<KVFile>(NULL);
  }

  // General pointer to data
  kv_file->data = buffer->data() + sizeof(KVHeader);
  kv_file->data_size = buffer->size() - sizeof(KVHeader);


  // If txt content
  if (kv_file->header_.isTxt()) {
    kv_file->kvs = NULL;
    kv_file->info = NULL;
    kv_file->kvs_index = NULL;
    return kv_file;
  }

  Data *key_data = ModulesManager::shared()->getData(kv_file->header_.keyFormat);
  Data *value_data = ModulesManager::shared()->getData(kv_file->header_.valueFormat);

  if (!key_data) {
    error.set(au::str("Unknown data type for key: %s", kv_file->header_.keyFormat));
    return au::SharedPointer<KVFile>(NULL);
  }

  if (!value_data) {
    error.set(au::str("Unknown data type for value: %s", kv_file->header_.valueFormat));
    return au::SharedPointer<KVFile>(NULL);
  }

  kv_file->key_   = (DataInstance * )key_data->getInstance();
  kv_file->value_ = (DataInstance * )value_data->getInstance();

  DataInstance *key   = kv_file->key_;
  DataInstance *value = kv_file->value_;

  // Alloc space for info and index vector
  kv_file->kvs = (KV *)malloc(sizeof(KV) * kv_file->header_.info.kvs);
  kv_file->info = (KVInfo *)malloc(sizeof(KVInfo) * KVFILE_NUM_HASHGROUPS);
  kv_file->kvs_index = (int *)malloc(sizeof(int) * KVFILE_NUM_HASHGROUPS);

  // Local pointer fo easy access
  KV *kvs = kv_file->kvs;
  KVInfo *info = kv_file->info;
  int *kvs_index = kv_file->kvs_index;

  for (int hg = 0; hg < KVFILE_NUM_HASHGROUPS; hg++) {
    info[hg].clear();
    kvs_index[hg] = -1;
  }

  // Point to the initial point of data content
  char *data = buffer->data() + sizeof(KVHeader);
  size_t offset = 0;
  int previous_hg = -1;
  KVInfo total_info;
  for (size_t i = 0; i < kv_file->header_.info.kvs; i++) {
    // Parsing key
    kvs[i].key = data + offset;
    kvs[i].key_size = key->parse(data + offset);
    offset += kvs[i].key_size;

    // Parsing value
    kvs[i].value = data + offset;
    kvs[i].value_size = value->parse(data + offset);
    offset += kvs[i].value_size;

    // Uppdate information of the correct hash-group
    int hg = key->hash(KVFILE_NUM_HASHGROUPS);
    info[hg].append(kvs[i].key_size + kvs[i].value_size, 1);
    // Check hg value
    if (hg < previous_hg) {
      error.set(
        au::str("Error parsing a block. Key-value #%lu belongs to hash-group %d and previous hg was %d"
                , i
                , hg
                , previous_hg));
      return au::SharedPointer<KVFile>(NULL);
    }

    // Update kvs_index if necessart
    if (hg != previous_hg) {
      kvs_index[hg] = i;
    }
    previous_hg = hg;

    // Update total counter ( for final check )
    total_info.append(kvs[i].key_size + kvs[i].value_size, 1);
  }

  // Check correct formatted block

  if (( total_info.size != kv_file->header_.info.size ) || ( total_info.kvs != kv_file->header_.info.kvs )) {
    error.set(au::str("Error creating KVInfo vector. %s != %s\n", total_info.str().c_str(),
                      kv_file->header_.info.str().c_str()));
    return au::SharedPointer<KVFile>(NULL);
  }

  // Check correct final offset
  if (offset != kv_file->header_.info.size) {
    error.set(au::str("Error parsing block. Wrong block size %lu != %lu\n", offset, kv_file->header_.info.size));
    return au::SharedPointer<KVFile>(NULL);
  }

  // Everything correct, return generated kv_file
  return kv_file;
}

KVFile::~KVFile() {
  if (info) {
    free(info);
    info = NULL;
  }

  if (kvs) {
    free(kvs);
    kvs = NULL;
  }

  if (kvs_index) {
    free(kvs_index);
    kvs_index = NULL;
  }
}

size_t KVFile::printContent(size_t limit, bool show_hg, std::ostream &output) {
  /*
   * if (strcmp(outputFormat, "plain") == 0)
   * {
   * output << "[" << time_string << "] " << key->str() << " " << value->str() << std::endl;
   * }
   * else if (strcmp(outputFormat, "json") == 0)
   * {
   * output << "[" << time_string << "] " << "{\"key\":" << key->strJSON() << ", \"value\":" << value->strJSON() << "}" << std::endl;
   * }
   * else if (strcmp(outputFormat, "xml") == 0)
   * {
   * output << "[" << time_string << "] " << "<?xml version=\"1.0\" encoding=\"UTF-8\" ?><key-value>\n<key>" << key->strXML() << "</key>\n<value>" << value->strXML() << "</value>\n</key-value>" << std::endl;
   * }
   *
   */


  if (header_.isTxt()) {
    size_t line_begin = 0;
    size_t num_lines = 0;
    while (true) {
      size_t line_size = 0;
      while (( data[line_begin + line_size ] != '\n') && ((line_begin + line_size) < data_size)) {
        line_size++;
      }

      std::string line;
      line.append(&data[line_begin], line_size);
      output << line << std::endl;

      line_begin += line_size + 1;

      num_lines++;
      if (limit > 0) {
        if (num_lines >= limit) {
          return num_lines;
        }
      }

      if (line_begin >= data_size) {
        return num_lines;
      }
    }
  }


  for (size_t i = 0; i < header_.info.kvs; i++) {
    key_->parse(kvs[i].key);
    value_->parse(kvs[i].value);

    if (show_hg) {
      output << "[ hg " << key_->hash(KVFILE_NUM_HASHGROUPS) << " ] ";
    }
    output << key_->str() << " " << value_->str() << std::endl;

    if (limit > 0) {
      if (i >= limit) {
        return i;
      }
    }
  }

  // All records have been printed out
  return header_.info.kvs;
}

// Get header information
KVHeader KVFile::header() {
  return header_;
}
}
