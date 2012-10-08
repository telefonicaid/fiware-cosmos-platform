
#ifndef _H_SAMSON_COMMON_COMMON
#define _H_SAMSON_COMMON_COMMON

#define MAX_UINT_32           4294967291
#define KVFILE_MAX_KV_SIZE    64 * 1024 * 1024                                            // Max size for an individual key-value
#define KVFILE_NUM_HASHGROUPS 64 * 1024                                                   // Number of hash-groups

#include <cstring>  // size_t

namespace samson {
// Unsigned types with different bits lengths

typedef size_t           uint64;
typedef unsigned int     uint32;
typedef unsigned short   uint16;
typedef unsigned char    uint8;
  
  
}

#endif

