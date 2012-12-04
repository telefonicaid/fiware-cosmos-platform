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

#include <sys/stat.h>  // mkdir

#include "engine/MemoryManager.h"  // samson::MemoryManager
#include "engine/Notification.h"                // engine::Notification

#include "engine/Buffer.h"  // engine::Buffer
#include "engine/DiskOperation.h"
#include "engine/Notification.h"                // engine::Notification

#include "samson/common/SamsonSetup.h"  // samson::SamsonSetup
// network::...
#include "samson/delilah/Delilah.h"  // samson::Delilah
#include "samson/network/Message.h"  // samson::Message
#include "samson/network/Packet.h"  // samson::Packet
// samson::MemoryInput , samson::MemoryOutput...

#include "PopDelilahComponent.h"  // Own interface

namespace samson {
PopDelilahComponent::PopDelilahComponent(std::string queue, std::string file_name, bool force_flag, bool show_flag)
  : DelilahComponent(DelilahComponent::pop) {
  queue_ = queue;
  file_name_ = file_name;
  force_flag_ = force_flag;
  show_flag_ = show_flag;

  // Main request information
  worker_id_ = static_cast<size_t>(-1);
  commit_id_ = SIZE_T_UNDEFINED;   // No previous commit observed

  // Init counter of items to be poped
  item_id_ = 1;

  // Default value
  num_pending_write_operations_ = 0;
  num_blocks_downloaded_ = 0;

  // Counter for responses we get
  num_pop_queue_responses_ = 0;
  started_ = false;

  // concept for this delilah component
  setConcept(au::str("Pop queue %s to local directory %s", queue.c_str(), file_name.c_str()));
}

PopDelilahComponent::~PopDelilahComponent() {
}

void PopDelilahComponent::run() {
  if (file_name_ != "") {  // Continuous pop operations
    if (force_flag_) {
      au::ErrorManager error;
      au::RemoveDirectory(file_name_, error);
    }

    if (mkdir(file_name_.c_str(), 0755) == -1) {
      LM_E(("Not possible to create directory. mkdir(%s): '%s'.", file_name_.c_str(), strerror(errno)));
      setComponentFinishedWithError(au::str("Not possible to create directory. mkdir(%s): '%s'."
                                            , file_name_.c_str(), strerror(errno)));
      return;
    }
  }

  // Send info request to a random worker
  SendMainRequest();
}

void PopDelilahComponent::review() {
  if (file_name_ == "") {
    SendMainRequest();  // If continuous, ask for more data
  } else if ((commit_id_ == SIZE_T_UNDEFINED) && (num_pop_queue_responses_ == 0) && (cronometer_.seconds() > 5)) {
    SendMainRequest();
  }
}

// Function to get the status
std::string PopDelilahComponent::getStatus() {
  if (num_pop_queue_responses_ == 0) {
    return au::str("Waiting for queue info from worker %lu", worker_id_);
  }

  return au::str("Downloading data form queue %s ( %lu pending blokcs ) "
                 , queue_.c_str(), items_.size());
}

std::string PopDelilahComponent::getExtraStatus() {
  au::tables::Table table("pop id|block id|Worker|Confirmed|Time|Buffer");

  table.setTitle("Items for this pop operation");

  au::map<size_t, PopDelilahComponentItem>::iterator it;
  for (it = items_.begin(); it != items_.end(); it++) {
    PopDelilahComponentItem *item = it->second;

    au::StringVector values;

    values.Push(item->pop_id());
    values.Push(item->block_id());


    values.Push(item->worker_id());

    if (item->worker_confirmation()) {
      values.Push("yes");
    } else {
      values.Push("no");
    } values.Push(au::str_time(item->cronometer().seconds()));

    if (item->buffer() == NULL) {
      values.Push("-");
    } else {
      values.Push(au::str(item->buffer()->size()));
    } table.addRow(values);
  }

  std::ostringstream output;
  output << "\n\n";

  if (num_pending_write_operations_ > 0) {
    output << "Waiting for " << num_pending_write_operations_ << " disk operations\n\n";
  }
  if (num_blocks_downloaded_ > 0) {
    output << "Total blocks downloaded " << num_blocks_downloaded_ << "\n\n";
  }
  output << table.str();
  output << "\n\n";
  return output.str();
}

void PopDelilahComponent::SendMainRequest() {
  cronometer_.Reset();

  size_t min_commit_id = SIZE_T_UNDEFINED;
  au::map< size_t, PopDelilahComponentItem >::iterator iter;
  for (iter = items_.begin(); iter != items_.end(); iter++) {
    if ((min_commit_id  == SIZE_T_UNDEFINED) || (iter->second->commit_id()  < min_commit_id)) {
      min_commit_id = iter->second->commit_id();
    }
  }

  worker_id_ = delilah->network->getRandomWorkerId(worker_id_);
  au::SharedPointer<Packet> packet(new Packet(Message::PopQueue));
  gpb::PopQueue *pop_queue = packet->message->mutable_pop_queue();
  pop_queue->set_queue(queue_);
  pop_queue->set_commit_id(commit_id_);
  pop_queue->set_min_commit_id(min_commit_id);

  // Identifier of the component at this delilah
  packet->message->set_delilah_component_id(id);

  // Information about direction
  packet->to = NodeIdentifier(WorkerNode, worker_id_);

  // Send message
  delilah->network->Send(packet);
  LOG_M(logs.delilah_components, ("pop request packet sent to worker_id_:%lu", worker_id_));
}

void PopDelilahComponent::receive(const PacketPointer& packet) {
  LOG_M(logs.delilah_components, ("Received a packet:%s", Message::messageCode(packet->msgCode)));
  // Message::PopQueueResponse
  // Response to main request. It provides list of blocks to be downloaded
  if (packet->msgCode == Message::PopQueueResponse) {
    if (!packet->message->has_pop_queue_response()) {
      LOG_SW(("Received a pop request response without correct information.Ignoring.."));
      return;
    }

    num_pop_queue_responses_++;

    // Add all element to the list
    const gpb::Queue& queue = packet->message->pop_queue_response().queue();
    LOG_M(logs.delilah_components, ("Received PopQueueResponse. Queue:'%s' blocks_size:%d, num_pop_queue_responses_:%d",
                                    queue.name().c_str(), queue.blocks_size(), num_pop_queue_responses_));
    LOG_M(logs.delilah_components, ("Message:'%s'", packet->message->ShortDebugString().c_str()));

    for (int i = 0; i < queue.blocks_size(); ++i) {
      size_t commit_id = queue.blocks(i).commit_id();
      if ((commit_id_ == SIZE_T_UNDEFINED) || (commit_id > commit_id_)) {
        commit_id_ = commit_id;  // Get the largest commit id
      }
      // Create a new item to be "poped"
      size_t block_id = queue.blocks(i).block_id();
      size_t item_id = item_id_++;
      PopDelilahComponentItem *item = new PopDelilahComponentItem(item_id, block_id, commit_id);
      items_.insertInMap(item_id, item);

      LOG_M(logs.delilah_components, ("Activate component with started flag"));
      set_started(true);

      // Send first request for this item
      LOG_M(logs.delilah_components, ("SendRequest called from PopDelilahComponent::receive(), first request"));
      SendRequest(item);

      // total counter of blocks
      num_blocks_downloaded_++;
    }

    LOG_M(logs.delilah_components,
          ("pop request response received (num_pop_queue_responses_:%d). Ready to check()", num_pop_queue_responses_));
    check();
    return;
  }

  // Message::PopBlockRequestConfirmation
  // response to confirm this block can or cannot be provided

  if (packet->msgCode == Message::PopBlockRequestConfirmation) {
    // Get identifier of the pop item it refers
    size_t pop_id = packet->message->pop_id();

    // Search for this item
    PopDelilahComponentItem *item = items_.findInMap(pop_id);
    if (!item) {
      return;
    }

    if (packet->message->has_error()) {
      SendRequest(item);
      check();
      return;
    } else
    if (item->worker_id() == packet->from.id) {
      item->SetWorkerConfirmation();
      check();
    }
    return;
  }

  // Message::PopBlockRequestResponse
  // response with the content of a block

  if (packet->msgCode == Message::PopBlockRequestResponse) {
    // Get identifier of the pop item it refers
    size_t pop_id = packet->message->pop_id();

    // Search for this item
    PopDelilahComponentItem *item = items_.findInMap(pop_id);
    if (!item) {
      LOG_SW(("Unknown pop item %lu in a PopBlockRequestResponse for delilah component %lu", pop_id, id));
      return;
    }

    item->SetContent(packet->buffer());
    check();
    return;
  }
}

void PopDelilahComponent::notify(engine::Notification *notification) {
  if (notification->isName(notification_disk_operation_request_response)) {
    num_pending_write_operations_--;
    check();
  } else {
    LOG_SW(("Unexpected notification %s", notification->name()));
  }
}

void PopDelilahComponent::check() {
  // Resent to other workers if necessary
  au::map<size_t, PopDelilahComponentItem>::iterator it;
  for (it = items_.begin(); it != items_.end(); it++) {
    PopDelilahComponentItem *item = it->second;
    if (item->buffer() == NULL) {
      int time_limit = 30;
      if (item->worker_confirmation()) {
        time_limit = 300;
      }
      if (item->cronometer().seconds() > time_limit) {
        SendRequest(item);
      }
    }
  }

  // Remove finish elements scheduling write operations
  for (it = items_.begin(); it != items_.end(); ) {
    PopDelilahComponentItem *item = it->second;

    // Get buffer for this item ( if available )
    engine::BufferPointer buffer = item->buffer();
    if (buffer == NULL) {
      return;
    }

    if (file_name_ != "") {
      // Write to disc
      std::string file_name = au::str("%s/file_%lu"
                                      , file_name_.c_str()
                                      , item->pop_id()
                                      );

      au::SharedPointer<engine::DiskOperation> operation
      (
        engine::DiskOperation::newWriteOperation(buffer, file_name, engine_id()));

      LOG_M(logs.delilah_components, ("Add write operation on file:'%s'", file_name.c_str()));
      engine::Engine::disk_manager()->Add(operation);
      num_pending_write_operations_++;
    } else {
      // Use delilah interface to report this block
      LOG_M(logs.delilah_components, ("Use delilah interface to report this block"));
      delilah->PublishBufferFromQueue(queue_, buffer);
    }

    // Remove this item in the list
    items_.erase(it++);
  }

  // Set component as finnish if everything has completed ( file_name is set in normal pop operations )
  if (file_name_ != "") {
    // Adding stated() to avoid finishing a component before started
    if (started() && (num_pending_write_operations_ == 0) && (items_.size() == 0)) {
      LOG_M(logs.delilah_components,
            (
              "pop operation finished on file '%s' because started:%d, num_pending_write_operations_:%d && items_.size():%lu",
              file_name_.c_str(), started_, num_pending_write_operations_, items_.size()));
      setComponentFinished();
    } else {
      LOG_M(logs.delilah_components,
            ("Waiting for pop component started:%d with num_pending_write_operations_:%d, items.size():%lu",
             started_, num_pending_write_operations_, items_.size()));
    }
  }
}

void PopDelilahComponent::SendRequest(PopDelilahComponentItem *item) const {
  item->ResetRequest();              // Reset request

  // Select a worker
  size_t worker_id = delilah->network->getRandomWorkerId(item->worker_id());
  item->set_worker_id(worker_id);

  // Build & sent the packet
  au::SharedPointer<Packet> packet(new Packet(Message::PopBlockRequest, NodeIdentifier(WorkerNode, worker_id)));

  // Fill information about this request
  packet->message->set_delilah_component_id(id);
  packet->message->set_pop_id(item->pop_id());
  packet->message->set_block_id(item->block_id());

  LOG_M(logs.delilah_components, ("pop request packet sent to worker_id_:%lu", worker_id_));
  delilah->network->Send(packet);
}
}
