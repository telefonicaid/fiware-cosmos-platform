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
#ifndef _H_BLOCK_MANAGER
#define _H_BLOCK_MANAGER

#include <list>
#include <set>
#include <string>
#include <vector>

#include "au/containers/Dictionary.h"
#include "au/containers/SharedPointer.h"

#include "engine/NotificationListener.h"              // engine::NotificationListener

#include "samson/common/Visualitzation.h"

#include "samson/stream/Block.h"                      // samson::stream::Block

namespace samson {
class SamsonWorker;
class GlobalBlockSortInfo;
namespace stream {
class BlockList;


/*
 *
 * BlockManager
 *
 * Manager of all the blocks running on the system
 * Main responsible to keep blocks of data in memory for operations
 *
 */

class BlockManager : public engine::NotificationListener {
public:

  // Singleton
  static void init();
  static BlockManager *shared();
  static void destroy();

  // Create blocks
  void CreateBlock(size_t block_id, engine::BufferPointer buffer);

  // Get a particular block
  BlockPointer GetBlock(size_t _id);

  // Reset the entire block manager
  void ResetBlockManager();

  // Function to review pending read / free / write operations
  void Review();

  // Notification interface
  virtual void notify(engine::Notification *notification);

  // Get collection of blocks for remote listing
  au::SharedPointer<gpb::Collection> GetCollectionOfBlocks(const Visualization& visualization);

  size_t scheduled_write_size() {
    return scheduled_write_size_;
  }

  size_t scheduled_read_size() {
    return scheduled_read_size_;
  }

  // Get all block identifiers
  std::set<size_t> GetBlockIds();

  // Check if all theses blocks are present
  bool CheckBlocks(const std::set<size_t>& block_ids);

  void set_samson_worker(SamsonWorker *samson_worker) {
    samson_worker_ = samson_worker;
  }

private:

  BlockManager();     // Private constructor & destructir for singleton implementation
  ~BlockManager();

  void Sort();     // Sort blocks
  void CreateBlockFromDisk(const std::string& path);
  void RecoverBlocksFromDisks();

  // Remove blocks not included in this list
  void RemoveBlocksIfNecessary(GlobalBlockSortInfo *info);

  void ScheduleRemoveOperation(BlockPointer block);
  void ScheduleReadOperation(BlockPointer block);
  void ScheduleWriteOperation(BlockPointer block);

  au::Dictionary<size_t, Block> blocks_;     // Dictionary of blocks
  std::list<size_t> block_ids_;     // list of block identifiers in order

  size_t scheduled_write_size_;     // Amount of bytes scheduled to be writen to disk
  size_t scheduled_read_size_;     // Amount of bytes scheduled to be read from disk
  size_t max_memory_;     // Maximum amount of memory to be used by this block manager

  au::Token token_;     // Mutex protection since operations create blocks in multiple threads

  SamsonWorker *samson_worker_;  // Pointer to samson worker
};
}
}

#endif  // ifndef _H_BLOCK_MANAGER
