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

#include "samson/module/Data.h"
#include "samson/module/ModulesManager.h"

#include "KVHeader.h"
#include "KVInfo.h"  // Own interface

namespace samson {
KVInfo::KVInfo(uint32 _size, uint32 _kvs) {
  kvs = _kvs;
  size = _size;
}

KVInfo::KVInfo() {
  kvs = 0;
  size = 0;
}

void KVInfo::set(uint32 _size, uint32 _kvs) {
  size = _size;
  kvs = _kvs;
}

void KVInfo::clear() {
  kvs = 0;
  size = 0;
}

void KVInfo::append(uint32 _size, uint32 _kvs) {
  size += _size;
  kvs += _kvs;
}

void KVInfo::append(const KVInfo& other) {
  size += other.size;
  kvs += other.kvs;
}

void KVInfo::remove(uint32 _size, uint32 _kvs) {
  size -= _size;
  kvs -= _kvs;
}

void KVInfo::remove(const KVInfo& other) {
  size -= other.size;
  kvs -= other.kvs;
}

std::string KVInfo::str() const {
  std::ostringstream o;

  o <<  "( " << au::str(kvs, "kvs") << " in " << au::str(size, "bytes") << " )";
  return o.str();
}

bool KVInfo::isEmpty() const {
  return ((kvs == 0) && (size == 0));
}

// Create the vector of KVInfo from block data
KVInfo *createKVInfoVector(char *_data, au::ErrorManager *error) {
  if (_data == NULL) {
    LM_E(("Null _data"));
    error->AddError(au::str("NULL _data"));
    return NULL;
  }

  KVHeader *header = reinterpret_cast<KVHeader *>(_data);
  char *data = _data + sizeof(KVHeader);

  Data *key_data = au::Singleton<ModulesManager>::shared()->GetData(header->keyFormat);
  Data *value_data = au::Singleton<ModulesManager>::shared()->GetData(header->valueFormat);

  if (!key_data) {
    LM_E(("Unknown data type %s", header->keyFormat));
    error->AddError(au::str("Unknown data type %s", header->keyFormat));
    return NULL;
  }

  if (!value_data) {
    LM_E(("Unknown data type %s", header->valueFormat));
    error->AddError(au::str("Unknown data type %s", header->valueFormat));
    return NULL;
  }

  DataInstance *key   = (DataInstance * )key_data->getInstance();
  DataInstance *value = (DataInstance * )value_data->getInstance();

  KVInfo *info = (KVInfo *)malloc(sizeof(KVInfo) * KVFILE_NUM_HASHGROUPS);

  for (int hg = 0; hg < KVFILE_NUM_HASHGROUPS; hg++) {
    info[hg].clear();
  }

  size_t offset = 0;
  int previous_hg = 0;
  for (size_t i = 0; i < header->info.kvs; i++) {
    size_t key_size   = key->parse(data + offset);
    offset += key_size;

    size_t value_size = value->parse(data + offset);
    offset += value_size;

    int hg = key->hash(KVFILE_NUM_HASHGROUPS);
    info[hg].append(key_size + value_size, 1);


    // Check hg value
    if (hg < previous_hg) {
      free(info);
      info = NULL;
      error->AddError(
        au::str(
          "Error getting KVInfo vector pargins %lu key-value. Current (%s) belongs to hg=%d and previous hg is %d"
          , i
          , key->str().c_str()
          , hg
          , previous_hg));
      return NULL;
    }

    previous_hg = hg;
  }


  // Check with header
  KVInfo total_info;
  total_info.clear();
  for (int hg = 0; hg < KVFILE_NUM_HASHGROUPS; hg++) {
    total_info.append(info[hg]);
  }

  if ((total_info.size != header->info.size) || (total_info.kvs != header->info.kvs)) {
    LM_X(1, ("Error creating KVInfo vector. %s != %s\n", total_info.str().c_str(), header->info.str().c_str()));
  }
  return info;
}
}
