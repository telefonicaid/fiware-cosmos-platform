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
#ifndef _H_SamsonClientBlockInterface_SamsonClient
#define _H_SamsonClientBlockInterface_SamsonClient

#include "samson/common/KVHeader.h"

namespace  samson {
// Class to work with blocks

class SamsonClientBlockInterface {
public:

  virtual ~SamsonClientBlockInterface() {
  }

  // Get header information about the block
  virtual KVHeader getHeader() = 0;

  // Get total size of the block
  virtual size_t bufferSize() = 0;
  virtual size_t getTXTBufferSize() = 0;


  // Get a pointer to data
  char *getData();


  // Get content
  virtual std::string get_header_content() = 0;
  virtual std::string get_content(int max_kvs, const char *format = "plain") = 0;
};
}

#endif  // ifndef _H_SamsonClientBlockInterface_SamsonClient
