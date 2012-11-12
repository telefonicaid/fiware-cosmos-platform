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
#include "au/string/xml.h"

#include "FullKVInfo.h"  // Own interface

namespace samson {
FullKVInfo::FullKVInfo(uint32 _size, uint32 _kvs) {
  kvs = _kvs;
  size = _size;
}

FullKVInfo::FullKVInfo() {
  kvs = 0;
  size = 0;
}

void FullKVInfo::set(uint32 _size, uint32 _kvs) {
  size = _size;
  kvs = _kvs;
}

void FullKVInfo::set(uint64 _size, uint64 _kvs) {
  size = _size;
  kvs = _kvs;
}

void FullKVInfo::clear() {
  kvs = 0;
  size = 0;
}

#ifdef __LP64__
void FullKVInfo::append(uint64 _size, uint64 _kvs) {
  size += _size;
  kvs += _kvs;
}

#endif

void FullKVInfo::append(const FullKVInfo& other) {
  size += other.size;
  kvs += other.kvs;
}

void FullKVInfo::Append(double factor, const KVInfo& other) {
  size += factor * (double)other.size;
  kvs += factor * (double)other.kvs;
}

#ifdef __LP64__
void FullKVInfo::remove(uint64 _size, uint64 _kvs) {
  size -= _size;
  kvs -= _kvs;
}

#endif

void FullKVInfo::remove(FullKVInfo other) {
  size -= other.size;
  kvs -= other.kvs;
}

#pragma mark ----

void FullKVInfo::append(uint32 _size, uint32 _kvs) {
  size += _size;
  kvs += _kvs;
}

void FullKVInfo::append(const KVInfo& other) {
  size += other.size;
  kvs += other.kvs;
}

void FullKVInfo::set(const KVInfo& other) {
  size = other.size;
  kvs = other.kvs;
}

void FullKVInfo::remove(uint32 _size, uint32 _kvs) {
  size -= _size;
  kvs -= _kvs;
}

void FullKVInfo::remove(const KVInfo& other) {
  size -= other.size;
  kvs -= other.kvs;
}

bool FullKVInfo::fitsInKVInfo() const {
  if (size >= MAX_UINT_32) {
    return false;
  }
  if (kvs >= MAX_UINT_32) {
    return false;
  }
  return true;
}

KVInfo FullKVInfo::getKVInfo() const {
  return KVInfo(size, kvs);
}

std::string FullKVInfo::str() const {
  return au::str(kvs, "kvs") + "/" + au::str(size, "B");
}

std::string FullKVInfo::strDetailed() const {
  return au::str("( %lu kvs [%s] in %lu bytes [%s] )"
                 , kvs
                 , au::str(kvs).c_str()
                 , size
                 , au::str(size).c_str()
                 );
}

void FullKVInfo::getInfo(std::ostringstream& output) const {
  au::xml_open(output, "kv_info");
  au::xml_simple(output, "kvs", kvs);
  au::xml_simple(output, "size", size);
  au::xml_close(output, "kv_info");
}

bool FullKVInfo::isEmpty() const {
  return ((kvs == 0) && (size == 0));
}
}