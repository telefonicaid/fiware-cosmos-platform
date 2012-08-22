
#include "samson/common/coding.h"

#include "logMsg/logMsg.h"      // LM_M

#include "Block.h"              // samson::Stream::Block
#include "BlockManager.h"       // samson::stream::BlockManager

#include "WorkerTaskBase.h"      // Own interface


namespace samson {
namespace stream {
WorkerTaskBase::WorkerTaskBase(size_t id)
  : block_list_container_(au::str("block lists for task %lu", id)) {
  // Set the id of this task
  id_ = id;

  // By default it is not ready
  ready_ = false;

  // Set initial state
  task_state_ = "Init";
}

WorkerTaskBase::~WorkerTaskBase() {
}

bool WorkerTaskBase::is_ready() {
  if (ready_) {
    return true;
  }

  // Check if all the input blocks are in memory
  ready_ = block_list_container_.is_content_in_memory();

  if (ready_) {
    // Lock all content in memory
    block_list_container_.lock_content_in_memory();

    // Change state to ready...
    SetTaskState("ready");
  }

  return ready_;
}

size_t WorkerTaskBase::get_id() {
  return id_;
}

void WorkerTaskBase::SetTaskState(const std::string& task_state) {
  task_state_ = task_state;
}

std::string WorkerTaskBase::task_state() {
  return task_state_;
}

void WorkerTaskBase::add_input(int channel, BlockPointer block, KVRange range, KVInfo info) {
  std::string block_list_name = au::str("input_%d", channel);
  BlockList *block_list = block_list_container_.getBlockList(block_list_name);

  block_list->add(new BlockRef(block, range, info));
}

void WorkerTaskBase::add_output(int channel, BlockPointer block, KVRange range, KVInfo info) {
  std::string block_list_name = au::str("output_%d", channel);
  BlockList *block_list = block_list_container_.getBlockList(block_list_name);

  block_list->add(new BlockRef(block, range, info));
}
}
}
