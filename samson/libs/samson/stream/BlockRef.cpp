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

#include "BlockRef.h"  // Own reference


#include <sstream>

#include "logMsg/logMsg.h"         // LOG_SM

#include "au/file.h"               // au::sizeOfFile

#include "engine/ProcessManager.h"  // engine::ProcessManager

#include "samson/common/EnvironmentOperations.h"    // getStatus()
#include "samson/common/SamsonSetup.h"
#include "samson/common/coding.h"  // KVInfo

#include "samson/module/ModulesManager.h"

#include "samson/network/NetworkInterface.h"
#include "samson/worker/SamsonWorker.h"

#include "Block.h"                 // samson::stream::Block
#include "BlockManager.h"   // samson::stream::BlockManager
#include "WorkerTask.h"

#include "BlockList.h"             // Own interface


namespace samson {
namespace stream {
BlockRef::BlockRef(BlockPointer block, KVRange range, KVInfo info) {
  block_ = block;
  range_ = range;
  info_ = info;
}

BlockRef::~BlockRef() {
}

BlockPointer BlockRef::block() {
  return block_;
}

size_t BlockRef::block_id() {
  return block_->block_id();
}

size_t BlockRef::block_size() {
  return block_->getSize();
}

KVInfo BlockRef::info() {
  return info_;
}

KVRange BlockRef::range() {
  return range_;
}

void BlockRef::append(BlockInfo& block_info) {
  // Information about number of blocks
  block_info.num_blocks++;

  // Information about sizes
  size_t size = block_->getSize();
  block_info.size += size;
  if (block_->is_content_in_memory()) {
    block_info.size_on_memory += size;
  }
  if (block_->is_content_in_disk()) {
    block_info.size_on_disk += size;
  }
  if (block_->is_content_locked_in_memory()) {
    block_info.size_locked += size;      // Key-Value information
  }
  block_info.info.append(info_);           // Only the key-values considered in this refernce

  block_info.push(block_->getKVFormat());

  block_info.pushTime(block_->getTime());
}

au::SharedPointer<KVFile> BlockRef::file() {
  return file_;
}

void BlockRef::review(au::ErrorManager& error) {
  if (!block_->is_content_in_memory()) {
    error.AddError(au::str("Block %lu is not in memory", block_->block_id()));
    return;
  }

  if (block_->getKVFormat().isTxt()) {
    // If data is txt, nothing else to do
    return;
  }

  // Get complete information about how key-values are organized in this block
  file_ = block_->getKVFile(error, false);      // Do not retain KVFile in the block
  if (file_ == NULL) {
    error.AddError(au::str("Not possible to parse block %lu", block_id()));
  }
}
}
}
