
#include <sys/stat.h>  // mkdir

#include "engine/MemoryManager.h"  // samson::MemoryManager
#include "engine/Notification.h"                // engine::Notification

#include "engine/Buffer.h"  // engine::Buffer
#include "engine/DiskOperation.h"
#include "engine/Notification.h"                // engine::Notificaiton

#include "samson/common/SamsonSetup.h"  // samson::SamsonSetup
#include "samson/common/samson.pb.h"  // network::...
#include "samson/delilah/Delilah.h"  // samson::Delilah
#include "samson/network/Message.h"  // samson::Message
#include "samson/network/Packet.h"  // samson::Packet
// samson::MemoryInput , samson::MemoryOutput...

#include "PopDelilahComponent.h"  // Own interface

namespace samson {
PopDelilahComponent::PopDelilahComponent(std::string queue
                                         , std::string file_name
                                         , bool force_flag
                                         , bool show_flag)
  : DelilahComponent(DelilahComponent::pop) {
  queue_ = queue;
  file_name_ = file_name;
  force_flag_ = force_flag;
  show_flag_ = show_flag;

  // Main request information
  worker_id_ = (size_t)-1;
  commit_id_ = -1;   // No previous commit observed

  // Init counter of items to be poped
  item_id_ = 1;

  // Default value
  num_pending_write_operations_ = 0;
  num_blocks_downloaded_ = 0;

  // Counter for responses we get
  num_pop_queue_responses_ = 0;

  // concept for this delilah component
  setConcept(au::str("Pop queue %s to local directory %s", queue.c_str(), file_name.c_str()));
}

PopDelilahComponent::~PopDelilahComponent() {
}

void PopDelilahComponent::run() {
  // Continuous pop operations
  if (file_name_ != "") {
    if (force_flag_) {
      au::ErrorManager error;
      au::removeDirectory(file_name_, error);
    }

    if (mkdir(file_name_.c_str(), 0755)) {
      setComponentFinishedWithError(au::str("Not possible to create directory %s (%s).", file_name_.c_str(),
                                            strerror(errno)));
      return;
    }
  }

  // Send info request to a random worker
  send_main_request();
}

void PopDelilahComponent::review() {
  if (file_name_ == "") {
    send_main_request();       // If continuous, ask for more data
  } else if (( commit_id_ = -1) && ( num_pop_queue_responses_ == 0 ) && ( cronometer_.seconds() > 5 )) {
    send_main_request();
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
  au::tables::Table table("pop id|block id|Ranges|Worker|Confirmed|Time|Buffer");

  table.setTitle("Items for this pop operation");

  au::map< size_t, PopDelilahComponentItem >::iterator it;
  for (it = items_.begin(); it != items_.end(); it++) {
    PopDelilahComponentItem *item = it->second;

    au::StringVector values;

    values.Push(item->pop_id());
    values.Push(item->block_id());
    values.Push(KVRanges(item->ranges()).str());


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
    output << "Total blocks downlaoded " << num_blocks_downloaded_ << "\n\n";
  }
  output << table.str();
  output << "\n\n";
  return output.str();
}

void PopDelilahComponent::send_main_request() {
  cronometer_.Reset();

  worker_id_ = delilah->network->getRandomWorkerId(worker_id_);
  au::SharedPointer<Packet> packet(new Packet(Message::PopQueue));
  gpb::PopQueue *pop_queue = packet->message->mutable_pop_queue();
  pop_queue->set_queue(queue_);
  pop_queue->set_commit_id(commit_id_);

  // Identifier of the component at this delilah
  packet->message->set_delilah_component_id(id);

  // Information about direction
  packet->to = NodeIdentifier(WorkerNode, worker_id_);

  // Send message
  delilah->network->Send(packet);
}

void PopDelilahComponent::receive(const PacketPointer& packet) {
  if (packet->msgCode == Message::PopQueueResponse) {
    if (!packet->message->has_pop_queue_response()) {
      LM_W(("Received a pop request response without correct information.Ignoring.."));
      return;
    }

    num_pop_queue_responses_++;

    // Add all element to the list
    const gpb::Queue& queue = packet->message->pop_queue_response().queue();
    for (int i = 0; i < queue.blocks_size(); i++) {
      int commit_id = queue.blocks(i).commit_id();
      if (commit_id > commit_id_) {
        commit_id_ = commit_id;
      }
      size_t block_id = queue.blocks(i).block_id();
      const gpb::KVRanges& ranges = queue.blocks(i).ranges();   // Implicit conversion

      size_t item_id = item_id_++;
      PopDelilahComponentItem *item = new PopDelilahComponentItem(item_id, block_id, ranges);
      items_.insertInMap(item_id, item);

      // Send first request for this item
      send_request(item);

      // total counter of blocks
      num_blocks_downloaded_++;
    }

    check();
    return;
  }

  // PopBlockRequestConfirmation

  if (packet->msgCode == Message::PopBlockRequestConfirmation) {
    // Get identifier of the pop item it refers
    size_t pop_id = packet->message->pop_id();

    // Search for this item
    PopDelilahComponentItem *item = items_.findInMap(pop_id);
    if (!item) {
      return;
    }

    if (packet->message->has_error()) {
      // Error in confirmation, send the next one
      send_request(item);
      check();
      return;
    } else
    if (item->worker_id() == packet->from.id) {
      item->SetWorkerConfirmation();
      check();
    }
    return;
  }

  // PopBlockRequestResponse

  if (packet->msgCode == Message::PopBlockRequestResponse) {
    // Get identifier of the pop item it refers
    size_t pop_id = packet->message->pop_id();

    // Search for this item
    PopDelilahComponentItem *item = items_.findInMap(pop_id);
    if (!item) {
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
  }

  LM_W(("Unexpected notification %s", notification->name()));
}

void PopDelilahComponent::check() {
  // Resent to other workers if necessry
  au::map< size_t, PopDelilahComponentItem >::iterator it;
  for (it = items_.begin(); it != items_.end(); it++) {
    PopDelilahComponentItem *item = it->second;
    if (item->buffer() == NULL) {
      int time_limit = 30;
      if (item->worker_confirmation()) {
        time_limit = 300;
      }
      if (item->cronometer().seconds() > time_limit) {
        send_request(item);
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
                                      , item->pop_id());

      au::SharedPointer< engine::DiskOperation> operation(engine::DiskOperation::newWriteOperation(buffer, file_name,
                                                                                                   engine_id()));
      engine::Engine::disk_manager()->Add(operation);
      num_pending_write_operations_++;
    } else {
      // Use delilah interface to report this block
      delilah->PublishBufferFromQueue(queue_, buffer);
    }

    // Remove this item in the list
    items_.erase(it++);
  }

  // Set component as finish if everything is done
  if (file_name_ != "") {
    if (( num_pending_write_operations_ == 0 ) && ( items_.size() == 0)) {
      setComponentFinished();
    }
  }
}
}
