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

#include "KVHeader.h"  // Own interface

namespace samson {
void KVHeader::Init(KVFormat format, KVInfo _info) {            // Complete init function
  magic_number =  4652783;

  SetKVFormat(format);
  info = _info;

  // Default initialization of the hash-group to full-files
  range.set(0, KVFILE_NUM_HASHGROUPS);

  // Current time-stamp
  time = ::time(NULL);
}

void KVHeader::InitForModule(size_t size) {
  magic_number =  4652783;
  SetKVFormat(KVFormat("module", "module"));

  // Full range to make sure it is in all workers
  range.set(0, KVFILE_NUM_HASHGROUPS);

  info.kvs = 0;    // In txt data-sets there are not "key-values"
  info.size = size;

  // Current time-stamp
  time = ::time(NULL);
}

void KVHeader::InitForTxt(size_t size) {
  magic_number =  4652783;
  SetKVFormat(KVFormat("txt", "txt"));

  // Random hash-group
  int hg = rand() % KVFILE_NUM_HASHGROUPS;
  range.set(hg, hg + 1);

  info.kvs = 0;  // In txt data-sets there are not "key-values"
  info.size = size;

  // Current time-stamp
  time = ::time(NULL);
}

bool KVHeader::IsTxt() {
  return KVFormat(keyFormat, valueFormat).isEqual(KVFormat("txt", "txt"));
}

bool KVHeader::IsModule() {
  return KVFormat(keyFormat, valueFormat).isEqual(KVFormat("module", "module"));
}

bool KVHeader::CheckTotalSize(size_t total_size) {
  return (total_size == ( sizeof(KVHeader) + info.size ));
}

void KVHeader::SetKVFormat(KVFormat format) {
  snprintf(keyFormat, 100, "%s", format.keyFormat.c_str());
  snprintf(valueFormat, 100, "%s", format.valueFormat.c_str());
}

// Format operations
// ---------------------------------------------------------------

KVFormat KVHeader::GetKVFormat() const {
  return KVFormat(keyFormat, valueFormat);
}

// Check operations ( magic number and other conditions )
// ---------------------------------------------------------------

bool KVHeader::Check() {
  return ( magic_number == 4652783);
}

std::string KVHeader::str() const {
  std::ostringstream output;

  output << "KVHeader: " << info.str() << "(" << range.str() << ") (" << keyFormat << "-" << valueFormat << ")";
  return output.str();
}
}
