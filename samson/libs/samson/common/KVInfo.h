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


  KVInfo(uint32 _size, uint32 _kvs);
  KVInfo();

  void set(uint32 _size, uint32 _kvs);
  void clear();
  void append(uint32 _size, uint32 _kvs);
  void append(KVInfo other);
  bool canAppend(KVInfo other);
  void remove(uint32 _size, uint32 _kvs);
  void remove(KVInfo other);
  std::string str();
  bool isEmpty();
  void getInfo(std::ostringstream& output);
};

// Create an allocated vector of KVInfo with information for all hash-groups
KVInfo *createKVInfoVector(char *data, au::ErrorManager *error);

// Get the agregation of
KVInfo selectRange(KVInfo *info, KVRange range);
}

#endif  // ifndef _H_SAMSON_KVINFO
