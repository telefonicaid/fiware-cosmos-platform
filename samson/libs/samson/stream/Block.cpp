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
#include "samson/stream/Block.h"                    // Own interface

#include "au/mutex/TokenTaker.h"
#include "au/string/S.h"

#include "engine/DiskManager.h"                     // notification_disk_operation_request_response
#include "engine/DiskOperation.h"                   // engine::DiskOperation
#include "engine/Engine.h"                          // engine::Engine
#include "engine/MemoryManager.h"                   // engine::MemoryManager
#include "engine/Notification.h"                    // engine::Notification
#include "engine/ProcessManager.h"

#include "logMsg/logMsg.h"                          // LOG_SW

#include "samson/common/KVHeader.h"
#include "samson/common/Logs.h"
#include "samson/common/MessagesOperations.h"
#include "samson/common/SamsonSetup.h"              // samson::SamsonSetup
#include "samson/module/KVFormat.h"
#include "samson/module/ModulesManager.h"           // ModulesManager
#include "samson/stream/BlockList.h"                // BlockList
#include "samson/stream/BlockManager.h"             // BlockManager
#include "samson/worker/SamsonWorkerController.h"

namespace samson {
namespace stream {
Block::Block(size_t block_id, engine::BufferPointer buffer) :
  token_lookupList("Block::token_lookupList"),
  token_file_("Block::token_file") {
  // Keep the buffer in this block
  buffer_ = buffer;

  // Default state is on_memory because the buffer has been given at memory ( input buffer )
  state_ = on_memory;

  // Unique identifier of the block across the cluster
  block_id_ = block_id;

  // Get a copy of the header
  memcpy(&header, buffer->data(), sizeof(KVHeader));

  // No lookup list by default
  lookupList = NULL;

  LOG_V(logs.block_manager, ("Block created from buffer: %s", str().c_str()));
}

Block::Block(size_t block_id, KVHeader *_header) :
  token_lookupList("token_lookupList"),
  token_file_("Block::token_file") {
  // Unique identifier of the block across the cluster
  block_id_ = block_id;

  // Default state is on_memory because the buffer has been given at memory
  state_ = on_disk;

  // Get a copy of the header
  memcpy(&header, _header, sizeof(KVHeader));

  // No llokup list by default
  lookupList = NULL;

  // Put cronometer to 1 hour before to remove blocks not included in the data model rigth now
  cronometer.AddOffset(-24 * 60 * 60);

  LOG_V(logs.block_manager, ("Block created from id: %s", this->str().c_str()));
}

Block::~Block() {
  if (lookupList) {
    delete lookupList;
    lookupList = NULL;
  }
}

au::SharedPointer<KVFile> Block::GetKVFile(au::ErrorManager& error) {
  au::TokenTaker tt(&token_file_);

  if (file_ != NULL) {
    return file_;  // If it has been calculated before, just return
  }

  if (buffer_ == NULL) {
    error.AddError(au::str("No buffer in memory for block %s", str_block_id(block_id_).c_str()));
    return au::SharedPointer<KVFile>(NULL);
  }

  // Create a new KVFile
  file_ = KVFile::create(buffer_, error);
  return file_;
}

Block::BlockState Block::state() const {
  return state_;
}

std::string Block::file_name() const {
  return au::Singleton<SamsonSetup>::shared()->block_filename(block_id_);
}

// Command over the block

void Block::freeBlock() {
  // Remove the lookup table if exist
  {
    au::TokenTaker tt(&token_lookupList);
    if (lookupList) {
      delete lookupList;
      lookupList = NULL;
    }
  }

  if (state_ != ready) {
    LOG_SW(("No sense to call free to a Block that state != ready "));
    return;
  }

  state_ = on_disk;

  if (buffer_ == NULL) {
    LOG_SW(("Buffer not present in a ready block"));
    return;
  }

  LOG_D(logs.block_manager, ("Destroying buffer for block:'%s'", str().c_str()));

  // Relase buffer
  buffer_.Reset();

  // Release KVFILE
  file_.Reset();
}

// Get information about this block
void Block::update(BlockInfo &block_info) {
  // Information about number of blocks
  block_info.num_blocks++;

  // Information about sizes
  block_info.size += header.info.size;
  if (is_content_in_memory()) {
    block_info.size_on_memory += header.info.size;
  }
  if (is_content_in_disk()) {
    block_info.size_on_disk += header.info.size;
  }
  // Key-Value information
  block_info.info.append(header.info);

  block_info.push(getKVFormat());

  block_info.pushTime(header.time);
}

size_t getSize(std::set<Block *> &blocks) {
  size_t total = 0;

  std::set<Block *>::iterator i;
  for (i = blocks.begin(); i != blocks.end(); i++) {
    total += (*i)->getSize();
  }

  return total;
}

std::string Block::str() {
  std::ostringstream output;

  output << "[ ";
  output << " id=" << str_block_id(block_id_) << " size=" << header.info.size << " " << header.range.str() << "(" <<
  str_state()
         << ")";
  output << " ]";
  return output.str();
}

// Function to check if this block can be removed from block manager ( basically it is not contained anywhere )
bool Block::canBeRemoved() {
  if (state_ == reading) {
    return false;
  }

  if (state_ == writing) {
    return false;
  }

  if (block_lists_.size() > 0) {
    return false;
  }

  if (block_lists_lock_.size() > 0) {
    return false;
  }

  return true;
}

bool Block::is_content_in_memory() const {
  return ((state_ == ready) || (state_ == on_memory) || (state_ == writing));
}

bool Block::is_content_in_disk() const {
  return ((state_ == ready) || (state_ == on_disk) || (state_ == reading));
}

size_t Block::getSize() const {
  return header.info.size + sizeof(KVHeader);
}

size_t Block::getSizeOnMemory() const {
  if (is_content_in_memory()) {
    return getSize();
  } else {
    return 0;
  }
}

size_t Block::getSizeOnDisk() const {
  if (is_content_in_disk()) {
    return getSize();
  } else {
    return 0;
  }
}

KVRange Block::getKVRange() const {
  return header.range;
}

KVInfo Block::getKVInfo() const {
  return header.info;
}

size_t Block::block_id() const {
  return block_id_;
}

KVFormat Block::getKVFormat() const {
  return header.GetKVFormat();
}

size_t Block::getTime() const {
  return header.time;
}

void Block::fill(samson::gpb::CollectionRecord *record, const Visualization& visualization, size_t accumulated_size) {
  samson::add(record, "block_id", str_block_id(block_id_), "left,different");

  samson::add(record, "size", getSize(), "f=uint64,sum");

  samson::add(record, "tsize", accumulated_size, "f=uint64,sum");

  if (visualization.get_flag("-info")) {
    samson::add(record, "state", str_state(), "left,different");
    samson::add(record, "info", info_state_, "left,different");
    return;
  }

  // Normal flags
  if (file_ == NULL) {
    samson::add(record, "Aux", "-", "f=uint64,sum");
  } else {
    samson::add(record, "Aux", file_->size(), "f=uint64,sum");
  }

  samson::add(record, "KVFormat", getKVFormat().str(), "left,different");
  samson::add(record, "KVRange", getKVRange().str(), "left,different");
  samson::add(record, "KVInfo", getKVInfo().str(), "left,different");

  samson::add(record, "state", str_state(), "left,different");

  // Time of creation
  samson::add(record, "created", au::S(cronometer).str(), "left,different");

  samson::add(record, "locked", is_content_locked_in_memory() ? "yes" : "no", "left,different");

  samson::add(record, "info", info_state_, "left,different");


  /*
   * // Next task
   * if (min_task_id_ == (size_t)(-1)) {
   * samson::add(record, "next task", "none", "left,different");
   * } else {
   * samson::add(record, "next task", min_task_id_, "left,different");       // Priority level
   * }
   *
   * samson::add(record, "priority", max_priority_, "left,different");
   *
   * // Lists
   * samson::add(record, "lists", block_lists_.size(), "left,different");
   *
   */
}

// au::Token token_lookupList;
// BlockLookupList* lookupList;

void Block::lookup(const char *key, au::SharedPointer<au::network::RESTServiceCommand> command) {
  au::TokenTaker tt(&token_lookupList);           // Mutex protection

  if (!lookupList) {
    lookupList = new BlockLookupList(this);

    // Detect error during creating
    if (lookupList->error.HasErrors()) {
      LM_E(("Error creating BlockLookupList (%s)", lookupList->error.GetLastError().c_str()));
      delete lookupList;
      lookupList = NULL;
      command->AppendFormatedError("Error creating BlockLookupList");
      return;
    }
  }

  lookupList->lookup(key, command);
}

std::string Block::str_state() const {
  std::ostringstream output;

  // Memory status
  if (is_content_locked_in_memory()) {
    output << "L";
  } else if (is_content_in_memory()) {
    output << "M";
  } else if (state_ == reading) {
    output << "R";
  } else {
    output << " ";           // Disk status
  }
  if (is_content_in_disk()) {
    output << "D";
  } else if (state_ == writing) {
    output << "W";
  } else {
    output << " ";
  }
  return output.str();
}

// Get the header
KVHeader Block::getHeader() const {
  return header;
}

bool Block::is_content_locked_in_memory() const {
  return (block_lists_lock_.size() > 0);
}

void Block::add_block_list(BlockList *list) {
  block_lists_.insert(list);
}

void Block::remove_block_list(BlockList *list) {
  // Remove from all the lists
  block_lists_.erase(list);
  block_lists_lock_.erase(list);
}

// lock content of this block in memory
void Block::lock_content_in_memory(BlockList *list) {
  block_lists_lock_.insert(list);
}

void Block::unlock_content_in_memory(BlockList *list) {
  block_lists_lock_.erase(list);
}

engine::BufferPointer Block::buffer() {
  return buffer_;
}

size_t Block::creation_time() const {
  return cronometer.seconds();
}

engine::BufferPointer Block::GetBufferFromDisk() {
  std::string source_file_name = file_name();
  size_t file_size = au::sizeOfFile(source_file_name);

  engine::BufferPointer buffer = engine::Buffer::Create(au::str("Block at file %s",
                                                                source_file_name.c_str()), file_size);
  au::ErrorManager error_writing_file;

  buffer->WriteFromFile(source_file_name, error_writing_file);

  if (error_writing_file.HasErrors()) {
    return engine::BufferPointer(NULL);
  }
  return buffer;
}
}
}
