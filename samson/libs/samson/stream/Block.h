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
#ifndef _H_BLOCK
#define _H_BLOCK

/* ****************************************************************************
 *
 * FILE                      Block.h
 *
 * AUTHOR                    Andreu Urruela Planas
 *
 * Basic unit of data for stream processing
 * A block is a unit of data possibly in memory with replication of disk
 *
 */

#include <set>
#include <string>

#include "au/containers/SharedPointer.h"
#include "au/containers/set.h"
#include "au/mutex/Token.h"
#include "au/mutex/TokenTaker.h"

#include "engine/DiskOperation.h"                   // engine::DiskOperation
#include "engine/MemoryManager.h"
#include "engine/NotificationListener.h"            // engien::EngineListener
#include "engine/ProcessItem.h"
#include "samson/common/FullKVInfo.h"
#include "samson/common/KVFile.h"
#include "samson/common/KVHeader.h"
#include "samson/common/KVInfo.h"
#include "samson/common/Visualitzation.h"
#include "samson/common/samson.pb.h"
#include "samson/module/KVSetStruct.h"              // samson::KVSetStruct
#include "samson/stream/BlockInfo.h"                // struct BlockInfo
#include "samson/stream/BlockLookupList.h"


namespace samson {
/**
 * Namespace for stream operations management in SAMSON library
 */
namespace stream {
class BlockList;


class Block {
public:
  typedef enum {
    on_memory,    // It is only in memory
    writing,      // A write order has been given
    ready,        // On disk and memory
    on_disk,      // Just on disk ( memory has been freed )
    reading       // Reading from disk to recover at memory
  } BlockState;

  Block(size_t block_id, engine::BufferPointer buffer);     // When block is in memory
  Block(size_t block_id, KVHeader *_header);     // When block is readed from disk
  ~Block();

  // Compute KVFile for this block ( if retain is true, it will keep for the next call )
  au::SharedPointer<KVFile> getKVFile(au::ErrorManager& error, bool retain);

  // Accessorrs
  BlockState state() const;
  std::string file_name() const;
  size_t creation_time() const;
  KVHeader getHeader() const;
  size_t getSize() const;
  size_t getSizeOnMemory() const;
  size_t getSizeOnDisk() const;
  KVFormat getKVFormat() const;
  size_t getTime() const;
  size_t block_id() const;
  KVRange getKVRange() const;
  KVInfo getKVInfo() const;
  std::string str_state() const;

  // Lookup for a particular queue ( lookupList is created if necessary )
  void lookup(const char *key, au::SharedPointer<au::network::RESTServiceCommand> command);

  // Handy function to check if this block can be removed from manager
  bool canBeRemoved();

  bool is_content_in_memory() const;            // Check if content is on memory
  bool is_content_in_disk() const;              // Check if content is on disk
  bool is_content_locked_in_memory() const;     // Check if block is locked in memory

  // Add to a list
  void add_block_list(BlockList *list);
  void remove_block_list(BlockList *list);

  // lock content of this block in memory
  void lock_content_in_memory(BlockList *list);
  void unlock_content_in_memory(BlockList *list);

  // Get information about this block
  void update(BlockInfo &block_info);

  // Free content from memory ( lookuplist is also removed )
  void freeBlock();

  // Debug string
  std::string str();

  // Collection-like monitorization information
  void fill(samson::gpb::CollectionRecord *record, const Visualization& visualization, size_t accumulated_size);

  // Get internal buffer with data
  engine::BufferPointer buffer();

  std::string info_state() {
    return info_state_;
  }

  void set_info_state(const std::string& value) {
    info_state_ = value;
  }

private:

  // Identifiers of this block
  size_t block_id_;

  // State disk-memory
  BlockState state_;

  // Debug string
  std::string info_state_;

  // Buffer of data ( if content of this block is on memory )
  engine::BufferPointer buffer_;

  // Copy of the header ( always in memory )
  KVHeader header;

  // Lock up table to quick access to key-values from REST interface
  au::Token token_lookupList;
  BlockLookupList *lookupList;

  // Creation time of this block
  au::Cronometer cronometer;

  // Containers of this block
  au::set<BlockList> block_lists_;     // List simply containing this block
  au::set<BlockList> block_lists_lock_;     // Lists locking this block in memory

  // Information about how key-values are organized in hash-groups
  au::SharedPointer<KVFile> file_;
  au::Token token_file_;

  friend class BlockManager;
  friend class BlockLookupList;
};

typedef au::SharedPointer<Block>   BlockPointer;
}
}

#endif  // ifndef _H_BLOCK
