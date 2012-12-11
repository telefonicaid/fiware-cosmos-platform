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
#ifndef _H_SAMSON_KVFILE
#define _H_SAMSON_KVFILE

#include "au/ErrorManager.h"
#include "engine/Buffer.h"
#include "samson/common/KVHeader.h"
#include "samson/common/KVInfo.h"
#include "samson/module/DataInstance.h"
#include "samson/module/KV.h"

namespace samson {
/**
 * Structure used to work with contents of KVFiles
 **/

class KVFile {
  KVFile() : auxiliar_buffer_(0) {
    // Default values ( for correct deallocation in case of errors )
    kvs = NULL;
    info = NULL;
    kvs_index = NULL;
    key_ = NULL;
    value_ = NULL;

  }

public:

  static au::SharedPointer<KVFile> create(engine::BufferPointer buffer, au::ErrorManager& error);
  ~KVFile();

  // Get header information
  KVHeader header();

  // Data instances for parsing and printing contente
  DataInstance *key_;
  DataInstance *value_;

  // Print content of key-values ( mainly drebugging )
  size_t printContent(size_t limit, bool show_hg, std::ostream &output);

  // Get key-values vector for a particular hash-group
  KV *kvs_for_hg(int hg) {
    if (kvs_index[hg] == -1)
      return NULL;
    return &kvs[kvs_index[hg]];
  }

  char *data_for_hg(int hg) {
    if (kvs_index[hg] == -1)
      return NULL;
    return kvs[kvs_index[hg]].key;
  }

  size_t size() {
    return auxiliar_buffer_.size();
  }

  // Main interface to interact with key-values contained in this block
  KV *kvs;            // Pointers to all key-values
  KVInfo *info;       // Vector with all KVInfos for each hash-group
  int *kvs_index;     // Index in the kvs vector for each hash-group

  // Main interface to interact with content if txt content is provided
  char *data;
  size_t data_size;

  void* key() { return key_; }
  
private:

  // Buffer of data ( retained by the shared reference counter )
  engine::BufferPointer buffer_;

  // Buffer to hold all data for this KVFile
  au::TemporalBuffer auxiliar_buffer_;

  // Copy of the heder contained in the buffer
  KVHeader header_;
};
}

#endif  // ifndef _H_SAMSON_KVFILE
