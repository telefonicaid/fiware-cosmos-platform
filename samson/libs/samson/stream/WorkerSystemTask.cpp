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
#include "samson/stream/WorkerSystemTask.h"                       // Own interface
#include <list>

#include "engine/Engine.h"
#include "engine/Notification.h"
#include "samson/common/Logs.h"
#include "samson/common/MessagesOperations.h"
#include "samson/common/NotificationMessages.h"
#include "samson/network/Packet.h"                  // network::Packet
#include "samson/stream/Block.h"                    // samson::stream::Block
#include "samson/stream/BlockList.h"                // samson::stream::BlockList
#include "samson/stream/BlockManager.h"
#include "samson/worker/SamsonWorker.h"

namespace samson {
namespace stream {
// ------------------------------------------------------------------------
//
// BlockRequestTask
//
// ------------------------------------------------------------------------


BlockRequestTask::BlockRequestTask(SamsonWorker *samson_worker
                                   , size_t id
                                   , size_t block_id
                                   , const std::vector<size_t>& worker_ids)
  : WorkerTaskBase(samson_worker
                   , id
                   , au::str("BlockDistribution %lu to workers %s", block_id, au::str(worker_ids).c_str())
                   , true     // Defined as simple task to be executed at engine main thread
                   ) {
  block_id_ = block_id;
  worker_ids_ = worker_ids;

  // Selected block
  block_ = stream::BlockManager::shared()->GetBlock(block_id);

  if (block_ == NULL) {
    LOG_W(logs.worker, ("Created a distribution task for block unknown %s", str_block_id(block_id_).c_str()));
  }
  AddInput(0, block_, block_->getKVRange(), block_->getKVInfo());

  std::string message = au::str("<< Block distribution %lu to workers %s >>", block_id, au::str(worker_ids_).c_str());
  environment().Set("worker_command_id", message);
}

// Virtual method from engine::ProcessItem
void BlockRequestTask::run() {
  if (block_ == NULL) {
    LOG_W(logs.worker, ("Running distribution task for block unknown %s", str_block_id(block_id_).c_str()));
    return;  // Nothing to distribute
  }
  // Block is supposed to be on memory since this is a task
  if (!block_->is_content_in_memory()) {
    LM_X(1, ("Internal error"));
  }

  LOG_V(logs.block_request, ("Answering block request for %s to workers %s"
                             , str_block_id(block_id_).c_str()
                             , au::str(worker_ids_).c_str()));

  // Send a packet to all selected workers
  for (size_t i = 0; i < worker_ids_.size(); ++i) {
    PacketPointer packet(new Packet(Message::BlockRequestResponse));
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
    LM_X(1, ("Internal error"));  // Get kv file for this block
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
}
}

