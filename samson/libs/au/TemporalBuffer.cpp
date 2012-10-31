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
#include "TemporalBuffer.h"  // Own interface
#include <stdlib.h>

namespace au {
TemporalBuffer::TemporalBuffer(size_t size) {
  data_ = (char *)malloc(size);
  size_ = size;
}

char *TemporalBuffer::data() {
  return data_;
}

size_t TemporalBuffer::size() {
  return size_;
}

TemporalBuffer::~TemporalBuffer() {
  if (data_) {
    free(data_);
  }
}
}
