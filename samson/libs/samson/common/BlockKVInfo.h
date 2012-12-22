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

#ifndef _H_SAMSON_BLOCKKVINFO
#define _H_SAMSON_BLOCKKVINFO

#include "samson/common/BlockKVInfo.h"
#include "samson/common/FullKVInfo.h"
#include "samson/common/KVInfo.h"
#include "samson/common/common.h"

namespace samson {
/**
 * BlockKVInfo
 * \brief Struct to keep information about a set of blocks
 */

struct BlockKVInfo {
  int num_blocks;                   // Number of blocks
  uint64 size;                      // Total size
  uint64 kvs;                       // Total number of kvs

  BlockKVInfo() {
    num_blocks = 0;
    size = 0;
    kvs = 0;
  }

  void AppendBlock(size_t _size, size_t _kvs) {
    ++num_blocks;
    size += _size;
    kvs += _kvs;
  }

  void AppendBlock(const FullKVInfo& info) {
    ++num_blocks;
    size += info.size;
    kvs += info.kvs;
  }

  void Append(const BlockKVInfo& info) {
    num_blocks += info.num_blocks;
    size += info.size;
    kvs = info.kvs;
  }

  std::string str() {
    return au::str(num_blocks, "Bl") + "/" + au::str(kvs, "kvs") + "/" + au::str(size, "B");
  }
};
}

#endif  // ifndef _H_SAMSON_FULLKVINFO