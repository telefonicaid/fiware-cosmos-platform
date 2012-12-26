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

/* ****************************************************************************
*
* FILE            SimpleBuffer.h
*
* AUTHOR          Andreu Urruela
*
* DATE            July 2011
*
* DESCRIPTION
*
* Simple struct containg information for a memory buffer
*
* ****************************************************************************/

#include <cstring>
#include <string>

#ifndef _H_SIMPLE_BUFFER
#define _H_SIMPLE_BUFFER

/**
 * Simple class to specify an allocated space in memory
 */

namespace engine {
class SimpleBuffer {
public:

  SimpleBuffer() {
    data_ = NULL;
    size_ = 0;
  }

  SimpleBuffer(char *data, size_t size) {
    data_ = data;
    size_ = size;
  }

  bool checkSize(size_t size) {
    return ( size_ >= size );
  }

  char *data() {
    return data_;
  }

  size_t size() {
    return size_;
  }

private:

  char *data_;
  size_t size_;
};
}

#endif  // ifndef _H_SIMPLE_BUFFER
