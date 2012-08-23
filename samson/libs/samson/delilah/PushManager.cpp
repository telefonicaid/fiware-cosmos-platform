
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

PushItem::PushItem(Delilah *delilah, size_t push_id, engine::BufferPointer buffer,
                   const std::vector<std::string>& queues) {
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
  state = init;
}

bool PushItem::isFinished() {
  return ( state == completed );
}

bool PushItem::isReadyForCommit() {
  return (state == ready_for_commit );
}

void PushItem::review() {
  if (state == init) {
    // Get a random worker id to push content
    worker_id_ = delilah_->network->getRandomWorkerId();
    if (worker_id_ == (size_t)-1) {
      return;   // no worker available...
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
    delilah_->network->Send(packet);


    // Change state to waiting push confirmation
    state = waiting_push_confirmation;
    return;
  }

  // Check if my worker is away and come back to init state if so
  if (!delilah_->network->isValidWorkerId(worker_id_)) {
    LM_W(("Worker %lu is part of the cluster anymore. Reseting push operation", worker_id_ ));
    state = init;
    return;
  }
}

void PushItem::reset() {
  if (state == completed) {
    return;
  }
  state = init;
}

// A Push response has been received from worker
void PushItem::receive(Message::MessageCode msgCode, size_t worker_id) {
  if (worker_id != worker_id_) {
    LM_W(("Received message %s from worker %lu in a push item while my worker id is %lu. Ignoring..."
          , Message::messageCode(msgCode)
          , worker_id
          , worker_id_ ));
    return;
  }

  if (state == init) {
    LM_W(("Received message %s in a push item while in init mode. Ignoring...", Message::messageCode(msgCode)));
    return;
  }

  if (state == waiting_push_confirmation) {
    if (msgCode != Message::PushBlockResponse) {
      LM_W(("Recieved wrong message (%s) from worker while waiting for distribution confirmation"
            , Message::messageCode(msgCode)));
      state = init;
      return;
    }

    // Change state to ready for commit
    state = ready_for_commit;

    return;
  }

  if (state == ready_for_commit) {
    LM_W(("Recieved wrong message (%s) from worker while being ready for commit"
          , Message::messageCode(msgCode)));
    state = init;
    return;
  }

  if (state == waiting_commig_confirmation) {
    if (msgCode != Message::PushBlockCommitResponse) {
      LM_W(("Recieved wrong message (%s) from worker while waiting for distribution confirmation"
            , Message::messageCode(msgCode)));
      state = init;
      return;
    }

    // Change state to completed
    state = completed;
  }
}

void PushItem::send_commit() {
  if (state != ready_for_commit) {
    return;
  }

  // Send the commit message
  PacketPointer packet(new Packet(Message::PushBlockCommit));
  packet->to = NodeIdentifier(WorkerNode, worker_id_);
  packet->message->set_push_id(push_id_);
  packet->set_buffer(buffer_);

  delilah_->network->Send(packet);

  state = waiting_commig_confirmation;
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

void PushManager::receive(Message::MessageCode msgCode, size_t worker_id, size_t push_id) {
  // Review push_items to deliver this message correctly...
  PushItem *item = items_.findInMap(push_id);

  if (item) {
    item->receive(msgCode, worker_id);
  } else {
    LM_W(("PushBlock response associated with an item (%lu) not found.", push_id ));  // Comit ready push operations and remove old connections
  }
  review();
}

void PushManager::reset(size_t push_id) {
  PushItem *item = items_.findInMap(push_id);

  if (item) {
    item->reset();
  } else {
    LM_W(("Canceling non-existing push operation with push_id = %lu", push_id ));
  }
}

size_t PushManager::push(engine::BufferPointer buffer, const std::vector<std::string>& queues) {
  LM_V(("PushManager: pushing buffer %s to %lu queues ( delilah %s )"
        , au::str(buffer->getSize()).c_str()
        , queues.size()
        , au::code64_str(delilah_->get_delilah_id()).c_str()
        ));

  size_t item_id = item_id_++;
  PushItem *item = new PushItem(delilah_, item_id, buffer, queues);
  items_.insertInMap(item_id, item);

  // First review to send packet to the server
  item->review();

  return item_id;
}

void PushManager::review() {
  // Review and remove finished
  au::map<size_t, PushItem>::iterator it;
  for (it = items_.begin(); it != items_.end(); it++) {
    PushItem *item = it->second;

    item->review();   // Review item

    if (item->isFinished()) {
      // Notification to inform that this push_id has finished
      engine::Notification *notification  = new engine::Notification("push_operation_finished");
      notification->environment().Set("push_id", it->first);
      notification->environment().Set("size",  item->size());
      engine::Engine::shared()->notify(notification);

      items_.erase(it);
    }
  }

  // Commit in order
  for (it = items_.begin(); it != items_.end(); it++) {
    PushItem *item = it->second;

    if (item->isReadyForCommit()) {
      item->send_commit();
    } else {
      break;
    }
  }
}

au::tables::Table *PushManager::getTableOfItems() {
  au::tables::Table *table = new au::tables::Table("Id|Buffer|Worker|Time|State");

  table->setTitle("push operations");

  au::map<size_t, PushItem>::iterator it;
  for (it = items_.begin(); it != items_.end(); it++) {
    PushItem *item = it->second;

    au::StringVector values;
    values.Push(item->get_push_id());
    values.Push(item->str_buffer_info());
    values.Push(item->worker_id_);
    values.Push(au::str_time(item->get_time()));
    values.Push(item->str());

    table->addRow(values);
  }

  return table;
}

size_t PushManager::get_num_items() {
  return items_.size();
}
}
