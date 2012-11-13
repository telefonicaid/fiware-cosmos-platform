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
#ifndef _H_SAMSON_COMMON_COMMON
#define _H_SAMSON_COMMON_COMMON

#include <cstring>  // size_t
#include "au/Log.h"
#include "au/log/LogMain.h"

#define MAX_UINT_32           4294967291
#define KVFILE_MAX_KV_SIZE    64 * 1024 * 1024                                            // Max size for an individual key-value
#define KVFILE_NUM_HASHGROUPS 64 * 1024                                                   // Number of hash-groups

namespace samson {
// Unsigned types with different bits lengths

typedef size_t uint64;
typedef unsigned int uint32;
typedef unsigned short uint16;
typedef unsigned char uint8;
  
  union BlockId
  {
    size_t uint64;
    unsigned int uint32[2];
  };
  
  std::string str_block_id(size_t block_id );

}
#endif

