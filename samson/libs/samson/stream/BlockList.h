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
#ifndef _H_BLOCK_LIST
#define _H_BLOCK_LIST

/* ****************************************************************************
 *
 * FILE                      BlockList.h
 *
 * AUTHOR                    Andreu Urruela Planas
 *
 * List of blocks: A tool for managing groups of blocks in queues, queue_operations, states, etc..
 *
 */

#include <list>
#include <ostream>                       // std::ostream
#include <string>                        // std::string

#include "au/containers/SharedPointer.h"
#include "au/containers/list.h"      // au::list
#include "au/containers/map.h"       // au::map
#include "au/statistics/Cronometer.h"      // au::cronometer
#include "engine/Buffer.h"      // engine::Buffer
#include "engine/NotificationListener.h"  // engine::NotificationListener
#include "samson/common/KVFile.h"
#include "samson/common/coding.h"    // FullKVInfo
// network::
#include "samson/stream/Block.h"
#include "samson/stream/BlockInfo.h"  // BlockInfo
#include "samson/stream/BlockRef.h"

namespace samson {
class Info;

namespace stream {
class Block;
class Queue;
class StreamManager;
class QueueItem;
class BlockMatrix;

class BlockList {
public:

  BlockList(std::string name = "no_name", size_t task_id = static_cast<size_t>(-1)) {
    name = name;
    task_id_ = task_id;     // Task is the order of priority
    lock_in_memory_ = false;     // By default no lock in memory
    priority_ = 0;     // Default priority level
  }

  ~BlockList();

  // Simple add or remove blocks
  void Add(BlockRef *block_ref);
  void Remove(BlockRef *block_ref);

  // Remove all the blocks contained in the list
  void clearBlockList();

  // Get information
  size_t getNumBlocks();

  void lock_content_in_memory();

  // Get information
  size_t task_id();
  int priority();

  // Get information about content included in this list
  BlockInfo getBlockInfo();

  // Review blocks to verify number of key-values
  void ReviewBlockReferences(au::ErrorManager& error);

  // string for debug blocks
  std::string str_blocks();

  // Check if it contains a particular block
  bool ContainsBlock(size_t block_id);

  std::vector<au::SharedPointer<KVFile> > GetKVFileVector(au::ErrorManager& error) {
    std::vector<au::SharedPointer<KVFile> > kv_files;
    au::list<BlockRef>::iterator bi;
    for (bi = blocks_.begin(); bi != blocks_.end(); ++bi) {
      BlockRef *block_ref = *bi;
      BlockPointer block = block_ref->block();
      engine::BufferPointer buffer = block->buffer();

      if (buffer == NULL) {
        error.AddError(au::str("Block %lu is apparently not in memory", block_ref->block_id()));
        kv_files.clear();
        return kv_files;
      }

      // Check header for valid block
      KVHeader *header = reinterpret_cast<KVHeader *> (buffer->data());
      if (!header->Check()) {
        error.AddError("Not valid header in block reference");
        kv_files.clear();
        return kv_files;
      }

      // Analyze all key-values and hashgroups
      au::SharedPointer<KVFile> file = block_ref->file();

      if (file == NULL) {
        error.AddError(au::str("Error getting KVFile for block %lu", block_ref->block_id()));
        kv_files.clear();
        return kv_files;
      }

      kv_files.push_back(file);
    }
    return kv_files;
  }

  // Check if content is in memory
  bool IsContentInMemory() const {
    au::list<BlockRef>::const_iterator it_blocks;
    for (it_blocks = blocks_.begin(); it_blocks != blocks_.end(); it_blocks++) {
      BlockRef *block_ref = *it_blocks;
      BlockPointer block = block_ref->block();
      if (!block->is_content_in_memory()) {
        return false;
      }
    }
    return true;
  }

  au::list<BlockRef> blocks_;  // Public manipulation of blocks for simplicity

private:

  std::string name_;     // Name of this block list ( for debugging )
  size_t task_id_;     // Order of the task if really a task
  bool lock_in_memory_;     // Lock in memory
  int priority_;     // Priority level for blocks that are not involved in tasks
};
}
}

#endif  // ifndef _H_BLOCK_LIST
