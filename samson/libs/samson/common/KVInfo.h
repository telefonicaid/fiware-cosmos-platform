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
#ifndef _H_SAMSON_KVINFO
#define _H_SAMSON_KVINFO

#include <sstream>
#include <string>

#include "au/ErrorManager.h"

#include "common.h"  // Basic types definition

#include "KVRange.h"

namespace samson {
/****************************************************************
*  KVInfo structure to keep information about size and # kvs
****************************************************************/

struct KVInfo {
  uint32 size;                  // Total size
  uint32 kvs;                   // Total number of kvs

  KVInfo();
  KVInfo(uint32 _size, uint32 _kvs);

  void set(uint32 _size, uint32 _kvs);
  void clear();
  void append(uint32 _size, uint32 _kvs);
  void append(const KVInfo& other);
  void remove(uint32 _size, uint32 _kvs);
  void remove(const KVInfo& other);
  bool isEmpty() const;
  std::string str() const;
};


// Create an allocated vector of KVInfo with information for all hash-groups
KVInfo *createKVInfoVector(char *data, au::ErrorManager *error);
}

#endif  // ifndef _H_SAMSON_KVINFO
