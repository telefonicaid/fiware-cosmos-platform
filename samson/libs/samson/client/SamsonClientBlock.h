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
#ifndef _H_SamsonClientBlock_SamsonClient
#define _H_SamsonClientBlock_SamsonClient

#include "samson/client/SamsonClientBlockInterface.h"
#include "samson/common/KVFile.h"

namespace  samson {
/*
 * class used to be able to read a engine::Buffer with live data from a queue
 */


class SamsonClientBlock : public SamsonClientBlockInterface {
  // Container of the in-memory buffer
  engine::BufferPointer buffer_;

  samson::KVHeader *header;            // Pointer to the header
  samson::KVFormat format;             // Format

  au::ErrorManager error;              // Error message

  char *data;                          // Pointer to data content

  au::SharedPointer<KVFile> kv_file;

public:

  SamsonClientBlock(engine::BufferPointer buffer) {
    // Create KVFile to manage this buffer
    kv_file = KVFile::create(buffer, error);

    // Keep block in memory
    buffer_ = buffer;

    // Header if always at the begining of the buffer
    header = (KVHeader *)buffer->data();

    // Check header
    if (!header->Check()) {
      error.AddError("Header check failed");
      return;
    }

    // Get the format from the header
    format =  header->GetKVFormat();

    size_t expected_size;

    if (format.isTxt()) {
      expected_size =   (size_t)(sizeof(samson::KVHeader)  + header->info.size);
      data = buffer->data() + sizeof(samson::KVHeader);
    } else {
      expected_size =   (size_t)(sizeof(samson::KVHeader) + header->info.size);
      data = buffer->data() + sizeof(samson::KVHeader);
    }

    if (expected_size != buffer->size()) {
      error.AddError("Wrong file format");
    }

    // Check if we have data types installed here
    samson::ModulesManager *modulesManager = au::Singleton<samson::ModulesManager>::shared();
    samson::Data *keyData = modulesManager->GetData(format.keyFormat);
    samson::Data *valueData = modulesManager->GetData(format.valueFormat);

    if (!format.isTxt()) {
      if (!keyData) {
        error.AddError(au::str("Unknown data type %s", format.keyFormat.c_str()));
      }

      if (!valueData) {
        error.AddError(au::str("Unknown data type %s", format.valueFormat.c_str()));
      }
    }
  }

  ~SamsonClientBlock() {
  }

  size_t bufferSize() {
    return buffer_->size();
  }

  size_t getTXTBufferSize() {
    return buffer_->size() - sizeof(samson::KVHeader);
  }

  char *getData() {
    return data;
  }

  KVHeader getHeader() {
    return *header;
  }

  std::string get_header_content() {
    if (error.HasErrors()) {
      return au::str("ERROR: %s\n", error.GetLastError().c_str());
    }




    return header->str() + "\n";
  }

  std::string get_content(int max_kvs, const char *outputFormat = "plain") {
    std::ostringstream output;

    if (kv_file != NULL) {
      kv_file->printContent(max_kvs, false, output);
    }
    return output.str();
  }
};
}

#endif  // ifndef _H_SamsonClientBlock_SamsonClient
