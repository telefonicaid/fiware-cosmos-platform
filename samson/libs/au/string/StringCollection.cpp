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

#include "logMsg/logMsg.h"

#include "StringCollection.h"  // Own interface

namespace au {
StringCollection::StringCollection() {
  v_ = (char *)malloc(1);
  size_ = 0;
  max_size_ = 1;
}

StringCollection::~StringCollection() {
  if (v_) {
    free(v_);
    v_ = NULL;
  }
}

int StringCollection::Add(const char *string) {
  // --------------------------------------------------------
  // Check previously introduced the same string
  // --------------------------------------------------------
  std::string _string = string;

  std::map<std::string, int>::iterator it = previous_strings.find(_string);
  if (it != previous_strings.end()) {
    return it->second;  // --------------------------------------------------------
  }
  size_t len = strlen(string);
  size_t required_size = size_ + len + 1;

  // Alloc more space if necessary
  if (required_size > max_size_) {
    size_t future_max_size = max_size_;
    while (future_max_size < required_size) {
      future_max_size *= 2;
    }

    char *vv = (char *)malloc(future_max_size);
    memcpy(vv, v_, size_);

    free(v_);
    v_ = vv;
    max_size_ = future_max_size;
  }

  int pos = (int)size_;

  memcpy(v_ + size_, string, len + 1);
  size_ += (len + 1);


  // Add in the map of preivous string
  previous_strings.insert(std::pair<std::string, int>(_string, pos));

  return pos;
}

size_t StringCollection::Write(FILE *file) {
  size_t t = fwrite(v_, size_, 1, file);

  if (t == 1) {
    return size_;
  } else {
    return 0;
  }
}

void StringCollection::Read(FILE *file, size_t _size) {
  // Free preivous buffer if any
  if (v_) {
    free(v_);
  }
  v_ = (char *)malloc(_size);
  size_ = _size;
  max_size_ = _size;

  // Read content from file
  size_t r = fread(v_, _size, 1, file);
  if (r != 1) {
    LM_W(("Error reading StringCollection"));
  }
}

size_t StringCollection::GetSize() {
  return size_;
}

const char *StringCollection::Get(int pos) {
  return &v_[pos];
}
}