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
#ifndef _H_SAMSON_FULLKVINFO
#define _H_SAMSON_FULLKVINFO

#include "samson/common/KVInfo.h"
#include "samson/common/common.h"

namespace samson {
/***********************************************************************
*  FullKVInfo (64 bits) structure to keep information about size and # kvs
***********************************************************************/

struct FullKVInfo {
  uint64 size;                  // Total size
  uint64 kvs;                   // Total number of kvs


  FullKVInfo(uint32 _size, uint32 _kvs);
  FullKVInfo();

  void set(KVInfo other);
  void set(uint32 _size, uint32 _kvs);
  void set(uint64 _size, uint64 _kvs);

  void clear();

  void append(uint64 _size, uint64 _kvs);
  void append(FullKVInfo other);
  void append(uint32 _size, uint32 _kvs);
  void append(KVInfo other);

  void remove(uint64 _size, uint64 _kvs);
  void remove(FullKVInfo other);
  void remove(uint32 _size, uint32 _kvs);
  void remove(KVInfo other);

  bool fitsInKVInfo();

  KVInfo getKVInfo();

  std::string str();
  std::string strDetailed();

  void getInfo(std::ostringstream& output);

  bool isEmpty();
};
}

#endif  // ifndef _H_SAMSON_FULLKVINFO
