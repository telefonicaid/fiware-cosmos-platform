
#include "engine/Engine.h"
#include "engine/Notification.h"

#include "samson/common/NotificationMessages.h"
// MemoryBlocks
#include "samson/common/MessagesOperations.h"

#include "samson/stream/BlockManager.h"
#include "samson/worker/SamsonWorker.h"

#include "samson/network/Packet.h"                  // network::Packet

#include "samson/module/ModulesManager.h"           // samson::ModulesManager


#include "Block.h"                                  // samson::stream::Block
#include "BlockList.h"                              // samson::stream::BlockList


#include "WorkerSystemTask.h"                       // Own interface

namespace samson {
namespace stream {
WorkerSystemTask::WorkerSystemTask(size_t id, const std::string& name, const std::string& concept)
  : engine::ProcessItem(PI_PRIORITY_NORMAL_OPERATION)
    , WorkerTaskBase(id, name , concept ) {
  // Keep internal values
  concept_ = concept;

  // Environment variables
  environment_.Set("system.task_id", id);
}

WorkerSystemTask::~WorkerSystemTask() {
}

std::string WorkerSystemTask::concept() {
  return concept_;
}

void WorkerSystemTask::fill(samson::gpb::CollectionRecord *record,
                            const Visualization& visualization) {
  if (visualization.get_flag("data")) {
    add(record, "id", get_id(), "left,different");

    for (int i = 0; i < 3; i++) {
      BlockList *block_list = block_list_container_.getBlockList(au::str("input_%d", i));
      BlockInfo block_info = block_list->getBlockInfo();
      add(record, au::str("Input %d", i), block_info.strShort(), "left,different");
    }
    for (int i = 0; i < 3; i++) {
      BlockList *block_list = block_list_container_.getBlockList(au::str("output_%d", i));
      BlockInfo block_info = block_list->getBlockInfo();
      add(record, au::str("Output %d", i), block_info.strShort(), "left,different");
    }

    return;
  }

  add(record, "id", get_id(), "left,different");
  add(record, "name", name(), "left,different");
  add(record, "worker_command_id", environment_.Get("worker_command_id", "?"), "left,different");
  add(record, "waiting", ProcessItem::waiting_time_seconds(), "f=time,different");
  add(record, "running ", ProcessItem::running_time_seconds(), "f=time,different");
  add(record, "progress ", progress(), "f=percentadge,different");

  if (ProcessItem::finished()) {
    add(record, "state", "finished", "left,different");
  } else if (ProcessItem::running()) {
    add(record, "state", "running", "left,different");
  } else {
    add(record, "state", task_state(), "left,different");
  }
}

// ------------------------------------------------------------------------
//
// BlockRequestTask
//
// ------------------------------------------------------------------------


BlockDistributionTask::BlockDistributionTask(size_t id
                                             , size_t block_id
                                             , const std::vector<size_t>& worker_ids)
  : WorkerSystemTask(id, "block_distribution", concept(block_id, worker_ids)) {
  block_id_ = block_id;
  worker_ids_ = worker_ids;

  // Selected block
  block_ = stream::BlockManager::shared()->getBlock(block_id);

  if (block_ == NULL) {
    LM_W(("DistributeBlockTask for unknown block %lu", block_id));
  }
  add_input(0, block_, block_->getKVRange(), block_->getKVInfo());

  std::string message = au::str("<< Block distribution %lu to workers %s >>"
                                , block_id
                                , au::str(worker_ids_).c_str());
  environment().Set("worker_command_id", message);
}

// Virtual method from engine::ProcessItem
void BlockDistributionTask::run() {
  if (block_ == NULL) {
    return;     // Nothing to distribute
  }
  // Block is suppouse to be on memory since this is a task
  if (!block_->is_content_in_memory()) {
    LM_X(1, ("Internal error"));
  }

  // Send a packet to all selected workers
  for (size_t i = 0; i < worker_ids_.size(); i++) {
    PacketPointer packet(new Packet(Message::BlockDistribution));
    packet->set_buffer(block_->buffer());
    packet->message->set_block_id(block_id_);
    packet->to = NodeIdentifier(WorkerNode, worker_ids_[i]);

    // Sending a engine notification to really sent this packet
    engine::Notification *notification = new engine::Notification(notification_send_packet);
    notification->dictionary().Set("packet",  packet.static_pointer_cast<NotificationObject>());
    engine::Engine::shared()->notify(notification);
  }
}

// ------------------------------------------------------------------------
//
// PopBlockRequestTask
//
// ------------------------------------------------------------------------

PopBlockRequestTask::PopBlockRequestTask(size_t id
                                         , size_t block_id
                                         , const gpb::KVRanges& ranges
                                         , size_t delilah_id
                                         , size_t delilah_component_id
                                         , size_t pop_id)
  : WorkerSystemTask(id, "pop_block_request", concept(block_id, delilah_id)) {
  block_id_ = block_id;
  ranges_.CopyFrom(ranges);

  delilah_id_ = delilah_id;
  delilah_component_id_ = delilah_component_id;
  pop_id_ = pop_id;

  // Selected block
  block_ = stream::BlockManager::shared()->getBlock(block_id);

  // add this block as input to make sure it is in memory when task is executed
  add_input(0, block_, block_->getKVRange(), block_->getKVInfo());
}

// Virtual method from engine::ProcessItem
void PopBlockRequestTask::run() {
  if (!block_->is_content_in_memory()) {
    LM_X(1, ("Internal error"));      // Get kv file for this block
  }
  au::SharedPointer<KVFile> kv_file = block_->getKVFile(error_);
  if (error_.IsActivated()) {
    LM_W(("Not possible to get KVFile for block in a PopBlockRequestTask"));
    return;
  }

  // Transform to c++ ranges class ( not gpb )
  KVRanges ranges(ranges_);      // Implicit conversion

  // Full range ( TBC )
  if (ranges.IsFullRange()) {
    sent_response(block_->buffer());
    return;
  }

  // txt-txt buffers
  if (kv_file->header().isTxt()) {
    // If ranges includes
    if (ranges.IsOverlapped(block_->getHeader().range)) {
      sent_response(block_->buffer());
    } else {
      sent_response(engine::BufferPointer(NULL));
    } return;
  }


  // Compute total size
  KVInfo info;
  for (int i = 0; i < KVFILE_NUM_HASHGROUPS; i++) {
    if (ranges.Contains(i)) {
      info.append(kv_file->info[i]);
    }
  }

  // Build selected packet filtering with ranges_
  size_t buffer_size = sizeof(KVHeader) + info.size;
  engine::BufferPointer buffer(engine::Buffer::Create("test", "example", buffer_size));
  buffer->set_size(buffer_size);

  // Copy header
  KVHeader header = block_->getHeader();
  header.info = info;      // Replaces with new info
  memcpy(buffer->data(), &header, sizeof(KVHeader));

  // Copy content for each hash-group

  char *source_data = kv_file->data;
  char *target_data = buffer->data() + sizeof(KVHeader);

  for (int i = 0; i < KVFILE_NUM_HASHGROUPS; i++) {
    size_t size = kv_file->info[i].size;

    if (ranges.Contains(i)) {
      memcpy(target_data, source_data, size);
      target_data += size;      // Move pointer to the next
    }
    source_data += size;      // Move pointer to the next
  }
  
  // Sent response with computed buffer
  sent_response( buffer );
  
}

void PopBlockRequestTask::sent_response(engine::BufferPointer buffer) {
  // Send a packet to delilah with generated content
  LM_W(("****** Sending PopBlockRequestResponse"));
  PacketPointer packet(new Packet(Message::PopBlockRequestResponse));

  packet->to = NodeIdentifier(DelilahNode, delilah_id_);
  packet->message->set_block_id(block_id_);
  packet->message->set_delilah_component_id(delilah_component_id_);
  packet->message->set_pop_id(pop_id_);
  packet->set_buffer(buffer);

  // Sending a engine notification to really sent this packet
  engine::Notification *notification = new engine::Notification(notification_send_packet);
  notification->dictionary().Set("packet",  packet.static_pointer_cast<NotificationObject>());
  engine::Engine::shared()->notify(notification);
}
}
}

