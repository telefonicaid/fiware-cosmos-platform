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

#include "samson/common/MessagesOperations.h"
#include "samson/delilah/Delilah.h"
#include "samson/network/Packet.h"

#include "PushManager.h"  // Own interface

namespace samson {
// ----------------------------------------------------
//
// PushItem
//
// ----------------------------------------------------

PushItem::PushItem(Delilah *delilah, size_t push_id
                   , engine::BufferPointer buffer
                   , const std::vector<std::string>& queues) {
  // Keep a pointer to delilah
  delilah_ = delilah;

  // Retain the buffer, just in case we have to resent
  buffer_ = buffer;

  // Identifier of this push block
  push_id_ = push_id;

  // Copy queues
  for (size_t i = 0; i < queues.size(); i++) {
    queues_.push_back(queues[i]);
  }

  // Default state
  state_ = init;
}

bool PushItem::IsFinished() {
  return (state_ == completed);
}

void PushItem::Review() {
  LOG_D(logs.delilah_push_manager, ("Review item %s", str().c_str()));

  if (state_ == completed) {
    return;   // Nothing to do
  }
  if (state_ == init) {
    worker_id_ = delilah_->network_->getRandomWorkerId();        // Get a random worker id to push content
    if (worker_id_ == static_cast<size_t>(-1)) {
      return;     // no worker available...
    }
    cronometer_.Reset();

    // Packet to send buffer to worker
    PacketPointer packet(new Packet(Message::PushBlock));
    packet->to = NodeIdentifier(WorkerNode, worker_id_);
    packet->message->set_push_id(push_id_);
    packet->set_buffer(buffer_);
    for (size_t i = 0; i < queues_.size(); i++) {
      packet->message->add_queue(queues_[i]);
    }

    // Send packet and wait for the answer
    delilah_->network_->Send(packet);

    // Change state to waiting push confirmation
    state_ = waiting_push_block_response;

    return;
  }

  // Check if my worker is away and come back to init state if so
  if (!delilah_->network_->IsWorkerConnected(worker_id_)) {
    LOG_SW(("Push item %lu: We are not connected to worker %lu anymore. Reseting item...", push_id_, worker_id_));
    state_ = init;
    return;
  }

  // Is worker is not part of the cluster
  if (!delilah_->network_->IsWorkerInCluster(worker_id_)) {
    LOG_SW(("Push item %lu: Worker %lu is not part of the cluster anymore. Reseting item...", push_id_, worker_id_));
    state_ = init;
    return;
  }

  if (cronometer_.seconds() > 30) {
    LOG_SW(("Push item %lu: Timeout 30 seconds. Reseting item...", push_id_, worker_id_));
    // Reset by time
    state_ = init;
  }
}

void PushItem::ResetPushItem() {
  if (state_ == completed) {
    return;
  }
  state_ = init;
}

// A Push response has been received from worker
void PushItem::receive(Message::MessageCode msgCode, size_t worker_id, au::ErrorManager& error) {
  if (state_ == completed) {
    return;  // This item is completed, nothing to do with me
  }

  if (worker_id != worker_id_) {
    LOG_SW(("Push item %lu: Received message '%s' from worker %lu in a push item (my worker id is %lu). Ignoring...",
            push_id_,
            Message::messageCode(msgCode),
            worker_id,
            worker_id_));
    return;
  }

  if (error.HasErrors()) {
    LOG_SW(("Push item %lu: Received error from worker '%s'. Reseting item...", push_id_, error.GetLastError().c_str()));
    ResetPushItem();
    return;
  }

  switch (state_) {
    case init:
      LOG_SW(("Push item %lu: Received message '%s' in a push item while in init mode. Ignoring..."
              , push_id_
              , Message::messageCode(msgCode)));
      break;

    case waiting_push_block_response:
      if (msgCode != Message::PushBlockResponse) {
        LOG_SW(("Push item %lu: Received wrong message '%s' from worker while waiting for PushBlockResponse"
                , push_id_
                , Message::messageCode(msgCode)));
        ResetPushItem();
        return;
      }
      state_ = waiting_push_block_confirmation;
      break;

    case waiting_push_block_confirmation:
      if (msgCode != Message::PushBlockConfirmation) {
        LOG_SW(("Push item %lu: Recieved wrong message '%s' from worker while waiting for PushBlockConfirmation"
                , push_id_
                , Message::messageCode(msgCode)));
        ResetPushItem();
        return;
      }
      state_ = completed;
      return;

      break;

    case completed:
      // Nothing else to do here
      break;
  }
}

size_t PushItem::push_id() const {
  return push_id_;
}

size_t PushItem::time() const {
  return cronometer_.seconds();
}

size_t PushItem::size() const {
  if (buffer_ != NULL) {
    return buffer_->size();
  } else {
    return 0;
  }
}

std::string PushItem::str() const {
  switch (state_) {
    case init: return au::str("[%lu] Uninitialized", push_id_);

    case waiting_push_block_response: return au::str("[%lu] Waiting push block response message from worker %lu"
                                                     , push_id_, worker_id_);

    case waiting_push_block_confirmation: return au::str("[%lu] Waiting push_commit confirmation from worker %lu"
                                                         , push_id_, worker_id_);

    case completed: return au::str("[%lu] Finalized", push_id_);
  }

  return "Error";
}

std::string PushItem::str_buffer_info() const {
  if (buffer_ != NULL) {
    return au::str(buffer_->size(), "B");
  } else {
    return "No buffer";
  }
}

// ----------------------------------------------------
//
// PushManager
//
// ----------------------------------------------------

PushManager::PushManager(Delilah *delilah) {
  delilah_ = delilah;
  item_id_ = 1;
}

void PushManager::receive(Message::MessageCode msgCode, size_t worker_id, size_t push_id, au::ErrorManager& error) {
  PushItem *item = items_.findInMap(push_id);

  if (item) {
    item->receive(msgCode, worker_id, error);
  } else {
    LOG_SW(("PushBlock response associated with an item (%lu) not found.", push_id));    // Comit ready push operations and remove old connections
  }

  Review();
}

size_t PushManager::Push(engine::BufferPointer buffer, const std::vector<std::string>& queues) {
  LOG_D(logs.delilah, ("PushManager: pushing buffer %s to %lu queues ( delilah %s )"
                       , au::str(buffer->size()).c_str()
                       , queues.size()
                       , au::code64_str(delilah_->delilah_id()).c_str()
                       ));

  size_t item_id = item_id_++;
  PushItem *item = new PushItem(delilah_, item_id, buffer, queues);
  items_.insertInMap(item_id, item);

  // First review to send packet to the server
  item->Review();

  return item_id;
}

void PushManager::Review() {
  LOG_D(logs.delilah_push_manager, ("Review items...."));

  au::map<size_t, PushItem>::iterator it;
  for (it = items_.begin(); it != items_.end(); ) {      // Review and remove finished items
    PushItem *item = it->second;
    item->Review();     // Review item

    if (item->IsFinished()) {
      // Notification to inform that this push_id has finished
      engine::Notification *notification  = new engine::Notification("push_operation_finished");
      notification->environment().Set("push_id", it->first);
      notification->environment().Set("size", item->size());
      engine::Engine::shared()->notify(notification);

      items_.erase(it++);
      delete item;   // Remove item itself
    } else {
      ++it;
    }
  }
}

void PushManager::ResetAllItems() {
  au::map<size_t, PushItem>::iterator it;
  for (it = items_.begin(); it != items_.end(); ++it) {
    PushItem *item = it->second;
    item->ResetPushItem();
  }
}

au::tables::Table *PushManager::getTableOfItems() {
  au::tables::Table *table = new au::tables::Table("Id|Buffer|Worker|Time|State");

  table->setTitle("push operations");

  au::map<size_t, PushItem>::iterator it;
  for (it = items_.begin(); it != items_.end(); ++it) {
    PushItem *item = it->second;

    au::StringVector values;
    values.Push(item->push_id());
    values.Push(item->str_buffer_info());
    values.Push(item->worker_id_);
    values.Push(au::str_time(item->time()));
    values.Push(item->str());

    table->addRow(values);
  }

  return table;
}

size_t PushManager::GetPendingSizeToPush() const {
  size_t total = 0;

  au::map<size_t, PushItem>::const_iterator iter;
  for (iter = items_.begin(); iter != items_.end(); ++iter) {
    total += iter->second->size();
  }
  return total;
}
}
