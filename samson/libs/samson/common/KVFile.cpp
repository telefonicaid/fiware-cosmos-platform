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

#include "au/string/StringUtilities.h"
#include "logMsg/logMsg.h"
#include <stdlib.h>  // malloc

#include "samson/common/Logs.h"
#include "samson/module/Data.h"
#include "samson/module/ModulesManager.h"


#include "KVFile.h"  // Own interface

namespace samson {
au::SharedPointer<KVFile> KVFile::create(engine::BufferPointer buffer, au::ErrorManager& error) {
  au::Cronometer cronometer;

  if (buffer == NULL) {
    error.AddError("NULL buffer provided");
    return au::SharedPointer<KVFile>(NULL);
  }

  // Candidate instance
  au::SharedPointer<KVFile> kv_file(new KVFile());
  kv_file->buffer_ = buffer;
  kv_file->key_    = NULL;
  kv_file->value_  = NULL;

  // buffer->SetTag( au::str("kvfile_%p" , kv_file.shared_object() ) );
  
  if (buffer->size() < sizeof(KVHeader)) {
    error.AddError(au::str("Incorrect buffer size (%lu) < header size ", buffer->size()));
    return au::SharedPointer<KVFile>(NULL);
  }

  // Copy & check header of this packet
  memcpy(&kv_file->header_, buffer->data(), sizeof(KVHeader));
  if (!kv_file->header_.Check()) {
    error.AddError("KVHeader error: wrong magic number");
    return au::SharedPointer<KVFile>(NULL);
  }

  // General pointer to data
  kv_file->data = buffer->data() + sizeof(KVHeader);
  kv_file->data_size = buffer->size() - sizeof(KVHeader);


  // If txt content
  if (kv_file->header_.IsTxt()) {
    kv_file->kvs = NULL;
    kv_file->info = NULL;
    kv_file->kvs_index = NULL;
    return kv_file;
  }

  Data *key_data = au::Singleton<ModulesManager>::shared()->GetData(kv_file->header_.keyFormat);
  Data *value_data = au::Singleton<ModulesManager>::shared()->GetData(kv_file->header_.valueFormat);

  if (!key_data) {
    error.AddError(au::str("Unknown data type for key: %s", kv_file->header_.keyFormat));
	LM_E(("Unknown data type for key: %s", kv_file->header_.keyFormat));
    return au::SharedPointer<KVFile>(NULL);
  }

  if (!value_data) {
    error.AddError(au::str("Unknown data type for value: %s", kv_file->header_.valueFormat));
    LM_E(("Unknown data type for value: %s", kv_file->header_.valueFormat));
    return au::SharedPointer<KVFile>(NULL);
  }

  kv_file->key_   = (DataInstance * )key_data->getInstance();
  kv_file->value_ = (DataInstance * )value_data->getInstance();

  DataInstance *key   = kv_file->key_;
  DataInstance *value = kv_file->value_;

  // Create a unified buffer to hold all auxiliar data required
  size_t size_for_kvs  = sizeof(KV) * kv_file->header_.info.kvs;
  size_t size_for_info = sizeof(KVInfo) * KVFILE_NUM_HASHGROUPS;
  size_t size_for_kvs_idnex = sizeof(int) * KVFILE_NUM_HASHGROUPS;
  size_t size_total = size_for_kvs + size_for_info + size_for_kvs_idnex;

  std::string buffer_name = au::str("KVFile for block %s", buffer->name().c_str());
  kv_file->auxiliar_buffer_ = engine::Buffer::Create(buffer_name, size_total);
  kv_file->auxiliar_buffer_->set_size(size_total);  // Set total size for statistics about used memory

  kv_file->kvs       = (KV *)kv_file->auxiliar_buffer_->data();
  kv_file->info      = (KVInfo *)(kv_file->auxiliar_buffer_->data() + size_for_kvs);
  kv_file->kvs_index = (int *)(kv_file->auxiliar_buffer_->data() + size_for_kvs + size_for_info);

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
      error.AddError(
        au::str("Error parsing a block. Key-value #%lu belongs to hash-group %d and previous hg was %d"
                , i
                , hg
                , previous_hg));

	  LM_E(("Error parsing a block."));
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

  if ((total_info.size != kv_file->header_.info.size) || (total_info.kvs != kv_file->header_.info.kvs)) {
    error.AddError(au::str("Error creating KVInfo vector. %s != %s\n", total_info.str().c_str(),
                           kv_file->header_.info.str().c_str()));
	LM_E(("Error creating KVInfo vector."));
    return au::SharedPointer<KVFile>(NULL);
  }

  // Check correct final offset
  if (offset != kv_file->header_.info.size) {
    error.AddError(au::str("Error parsing block. Wrong block size %lu != %lu\n", offset, kv_file->header_.info.size));
	LM_E(("Error parsing block."));
    return au::SharedPointer<KVFile>(NULL);
  }

  // Check correct offset
  if ((sizeof(KVHeader) + offset) != buffer->size()) {
    error.AddError(au::str("Error parsing block. Wrong block size %lu != %lu\n", offset, kv_file->header_.info.size));
    LM_E(("Error parsing block. Wrong buffer size %lu != %lu", buffer->size(), offset + sizeof(KVHeader)));
    return au::SharedPointer<KVFile>(NULL);
  }

  // Everything correct, return generated kv_file
  LOG_M(logs.kv_file, ("Created KVFile (%s) in %s using <%s,%s> for buffer %s"
                       , au::str(size_total, "B").c_str()
                       , au::str(cronometer.seconds()).c_str()
                       , kv_file->header_.keyFormat
                       , kv_file->header_.valueFormat
                       , buffer->str().c_str()));

  return kv_file;
}

KVFile::~KVFile() {
  if (key_) {
    delete key_;
    key_ = NULL;
  }
  if (value_) {
    delete value_;
    value_ = NULL;
  }
}

size_t KVFile::printContent(size_t limit, bool show_hg, std::ostream &output) {
  if (header_.IsTxt()) {
    size_t line_begin = 0;
    size_t num_lines = 0;
    while (true) {
      size_t line_size = 0;
      while ((data[line_begin + line_size ] != '\n') && ((line_begin + line_size) < data_size)) {
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
