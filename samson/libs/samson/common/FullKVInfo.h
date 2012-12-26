#ifndef SAMSON_LIBS_SAMSON_COMMON_FULLKVINFO_H_
#define SAMSON_LIBS_SAMSON_COMMON_FULLKVINFO_H_

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
#include <string>

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

  void set(const KVInfo& other);
  void set(uint32 _size, uint32 _kvs);
  void set(uint64 _size, uint64 _kvs);

  void clear();

  void append(uint64 _size, uint64 _kvs);
  void append(const FullKVInfo& other);
  void append(uint32 _size, uint32 _kvs);
  void append(const KVInfo& other);

  void Append(double factor, const KVInfo& other);

  void remove(uint64 _size, uint64 _kvs);
  void remove(FullKVInfo other);
  void remove(uint32 _size, uint32 _kvs);
  void remove(const KVInfo& other);

  bool fitsInKVInfo() const;

  KVInfo getKVInfo() const;

  std::string str() const;
  std::string strDetailed() const;

  void getInfo(std::ostringstream& output) const;

  bool isEmpty() const;
};
}

#endif  // SAMSON_LIBS_SAMSON_COMMON_FULLKVINFO_H_
