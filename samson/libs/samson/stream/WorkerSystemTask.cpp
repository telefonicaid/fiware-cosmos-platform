#include "samson/stream/WorkerSystemTask.h"                       // Own interface
#include <list>

#include "engine/Engine.h"
#include "engine/Notification.h"
#include "samson/common/MessagesOperations.h"
#include "samson/common/NotificationMessages.h"
#include "samson/network/Packet.h"                  // network::Packet
#include "samson/stream/Block.h"                                  // samson::stream::Block
#include "samson/stream/BlockList.h"                              // samson::stream::BlockList
#include "samson/stream/BlockManager.h"
#include "samson/worker/SamsonWorker.h"

namespace samson {
namespace stream {

// ------------------------------------------------------------------------
//
// BlockRequestTask
//
// ------------------------------------------------------------------------


BlockDistributionTask::BlockDistributionTask(SamsonWorker *samson_worker, size_t id, size_t block_id,
                                             const std::vector<size_t>& worker_ids)
     : WorkerTaskBase(samson_worker, id,
                      au::str("BlockDistribution %lu to workers %s", block_id, au::str(worker_ids).c_str())) {
  block_id_ = block_id;
  worker_ids_ = worker_ids;

  // Selected block
  block_ = stream::BlockManager::shared()->GetBlock(block_id);

  if (block_ == NULL) {
    LM_W(("DistributeBlockTask for unknown block %lu", block_id));
  }
  AddInput(0, block_, block_->getKVRange(), block_->getKVInfo());

  std::string message = au::str("<< Block distribution %lu to workers %s >>", block_id, au::str(worker_ids_).c_str());
  environment().Set("worker_command_id", message);
}

// Virtual method from engine::ProcessItem
void BlockDistributionTask::run() {
  if (block_ == NULL) {
    return; // Nothing to distribute
  }
  // Block is supposed to be on memory since this is a task
  if (!block_->is_content_in_memory()) {
    LM_X(1, ("Internal error"));
  }

  // Send a packet to all selected workers
  for (size_t i = 0; i < worker_ids_.size(); ++i) {
    PacketPointer packet(new Packet(Message::BlockDistribution));
    packet->set_buffer(block_->buffer());
    packet->message->set_block_id(block_id_);
    packet->to = NodeIdentifier(WorkerNode, worker_ids_[i]);

    // Sending a engine notification to really sent this packet
    engine::Notification *notification = new engine::Notification(notification_send_packet);
    notification->dictionary().Set<Packet> ("packet", packet);
    engine::Engine::shared()->notify(notification);
  }
}

// ------------------------------------------------------------------------
//
// PopBlockRequestTask
//
// ------------------------------------------------------------------------

PopBlockRequestTask::PopBlockRequestTask(SamsonWorker *samson_worker, size_t id, size_t block_id, size_t delilah_id,
                                         size_t delilah_component_id, size_t pop_id) :
  WorkerTaskBase(samson_worker, id, au::str("BlockPopRequest %lu [ delilah %lu ]", block_id, delilah_id)) {
  block_id_ = block_id;
  delilah_id_ = delilah_id;
  delilah_component_id_ = delilah_component_id;
  pop_id_ = pop_id;

  // Selected block
  block_ = stream::BlockManager::shared()->GetBlock(block_id);

  // add this block as input to make sure it is in memory when task is executed
  AddInput(0, block_, block_->getKVRange(), block_->getKVInfo());
}

// Virtual method from engine::ProcessItem
void PopBlockRequestTask::run() {
  if (!block_->is_content_in_memory()) {
    LM_X(1, ("Internal error")); // Get kv file for this block
  }

  // Send a packet to delilah with contents of this buffer
  PacketPointer packet(new Packet(Message::PopBlockRequestResponse));

  packet->to = NodeIdentifier(DelilahNode, delilah_id_);
  packet->message->set_block_id(block_id_);
  packet->message->set_delilah_component_id(delilah_component_id_);
  packet->message->set_pop_id(pop_id_);
  packet->set_buffer(block_->buffer());

  // Sending a engine notification to really sent this packet
  engine::Notification *notification = new engine::Notification(notification_send_packet);
  notification->dictionary().Set<Packet> ("packet", packet);
  engine::Engine::shared()->notify(notification);
}

void DefragTask::AddOutputBuffer(engine::BufferPointer buffer) {
  // Information for generated block
  KVHeader *header = reinterpret_cast<KVHeader *> (buffer->data());

  // Create a block ( and distribute it )
  size_t block_id = samson_worker_->worker_block_manager()->CreateBlock(buffer);
  BlockPointer block = BlockManager::shared()->GetBlock(block_id);

  // Add output to this operation
  AddOutput(0, block, header->range, header->info);
}

void DefragTask::run() {
  // Review input blocks
  BlockList *list = block_list_container_.getBlockList("input_0");
  list->ReviewBlockReferences(error_);

  if (error_.IsActivated()) {
    AU_LM_W((">>>> Error in defrag operation: %s" , error_.GetMessage().c_str() ));
    return;
  }

  // Get vector with all KVFiles at the input
  // ------------------------------------------------------------------------------
  std::vector<au::SharedPointer<KVFile> > kv_files;
  au::list<BlockRef>::iterator bi;
  for (bi = list->blocks.begin(); bi != list->blocks.end(); ++bi) {
    BlockRef *block_ref = *bi;
    BlockPointer block = block_ref->block();
    engine::BufferPointer buffer = block->buffer();

    if (buffer == NULL) {
      error_.set(au::str("Block %lu is apparently not in memory", block_ref->block_id()));
      return;
    }

    // Check header for valid block
    KVHeader *header = reinterpret_cast<KVHeader *> (buffer->data());
    if (!header->check()) {
      error_.set("Not valid header in block reference");
      return;
    }

    // Analyze all key-values and hashgroups
    au::SharedPointer<KVFile> file = block_ref->file();

    if (file == NULL) {
      error_.set(au::str("Error getting KVFile for block %lu", block_ref->block_id()));
      return;
    }

    kv_files.push_back(file);
  }

  if (kv_files.size() == 0) {
    error_.set("No data provided for defrag operation");
    return;
  }

  // Generate output buffers putting together all data for each hash-group
  // ------------------------------------------------------------------------------

  KVHeader header = kv_files[0]->header();

  for (size_t r = 0; r < ranges_.size(); ++r) {

    // Generate block ( if any data is present for this range )
    KVRange range = ranges_[r];

    // Compute info for this range
    KVInfo info;
    for (int hg = range.hg_begin_; hg < range.hg_end_; ++hg) {
      for (int i = 0; i < (int) kv_files.size(); ++i) {
        info.append(kv_files[i]->info[hg]);
      }
    }

    if (info.size == 0)
      continue;

    // Create output buffer ( hg_begin hg_end )
    size_t buffer_size = sizeof(KVHeader) + info.size;
    engine::BufferPointer buffer = engine::Buffer::Create("defrag", "normal", buffer_size);
    buffer->set_size(buffer_size);

    // Copy header and modify info
    KVHeader* buffer_header = reinterpret_cast<KVHeader*> (buffer->data());
    memcpy(buffer_header, &header, sizeof(KVHeader));
    buffer_header->info = info;
    buffer_header->range = range; // Set this range ( it could be better adjusted )

    // Copy data
    char* buffer_data = buffer->data();
    size_t offset = sizeof(KVHeader);
    for (int hg = range.hg_begin_; hg < range.hg_end_; ++hg) {
      for (size_t i = 0; i < kv_files.size(); ++i) {
        size_t size = kv_files[i]->info[hg].size;
        if (size > 0) {
          char* data = kv_files[i]->data_for_hg(hg);
          memcpy(buffer_data + offset, data, size);
          offset += size;
        }
      }
    }

    if (offset != buffer_size) {
      error_.set("Internal error in defrag operation");
      return;
    }

    // add generated buffer to the output
    AddOutputBuffer(buffer);
  }
}

std::string DefragTask::commit_command() {
  std::vector<std::string> inputs;
  std::vector<std::string> outputs;
  inputs.push_back(queue_name_);
  outputs.push_back(queue_name_);
  return generate_commit_command(inputs, outputs);
}
}
}

