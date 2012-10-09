#include "samson/worker/PushOperation.h"  // Own interface

#include "samson/network/Packet.h"
#include "samson/stream/BlockManager.h"
#include "samson/worker/SamsonWorker.h"
#include "samson/zoo/CommitCommand.h"

namespace samson {
PushOperation::PushOperation(SamsonWorker *samson_worker, size_t block_id, size_t block_size, size_t delilah_id,
                             size_t push_id, engine::BufferPointer buffer, const std::vector<std::string>& queues) {
  samson_worker_ = samson_worker;

  block_id_ = block_id;
  block_size_ = block_size;
  delilah_id_ = delilah_id;
  push_id_ = push_id;
  for (size_t i = 0; i < queues.size(); i++) {
    queues_.push_back(queues[i]);
  }

  // Copy the header of this block ( for keep information )
  memcpy(&header, buffer->data(), sizeof(KVHeader));

  // Receive notifications when a block has been correctly distributed
  listen("notification_block_correctly_distributed");

  // It is not finish by default
  distributed_ = false;
  commited_ = false;
}

size_t PushOperation::get_delilah_id() const {
  return delilah_id_;
}

size_t PushOperation::get_push_id() const {
  return push_id_;
}

size_t PushOperation::time() const {
  return static_cast<size_t>(cronometer_.seconds());
}

void PushOperation::notify(engine::Notification *notification) {
  if (!notification->isName("notification_block_correctly_distributed")) {
    LM_W(("Unexpected notification at PushOperation"));
    return;
  }

  if (distributed_) {
    return;   // If the block has already been distributed, ignore notifications
  }

  // Get block_id that has been distributed
  size_t block_id = notification->environment().Get("block_id", (size_t) -1);

  // If this is me
  if (block_id != (size_t) -1) {
    if (block_id == block_id_) {
      // Send a message to delilah
      PacketPointer packet(new Packet(Message::PushBlockResponse));
      packet->to = NodeIdentifier(DelilahNode, delilah_id_);
      packet->message->set_push_id(push_id_);
      samson_worker_->network()->Send(packet);

      // Set as distributed
      distributed_ = true;
    }
  }
}

void PushOperation::commit() {
  if (!distributed_) {
    LM_W(("Received a commit in a Push operation wihtout distributing previously. Ignoring... "));
    return;
  }

  if (commited_) {
    LM_W(("Received a double commit in a Push operation. Ignoring... "));
    return;
  }

  // Prepare commit command
  CommitCommand commit_command;
  for (size_t i = 0; i < queues_.size(); i++) {
    commit_command.AddBlock(queues_[i], block_id_, block_size_, header.getKVFormat(), header.range, header.info);
  }
  std::string command = commit_command.GetCommitCommand();

  // Commit
  au::ErrorManager error;
  std::string caller = au::str("PushOperation from delilah %s ( push id %lu )", au::code64_str(delilah_id_).c_str(),
                               push_id_);

  samson_worker_->data_model()->Commit(caller, command, &error);

  // Answer message to delilah
  PacketPointer packet(new Packet(Message::PushBlockCommitResponse));
  packet->to = NodeIdentifier(DelilahNode, delilah_id_);
  packet->message->set_push_id(push_id_);

  if (error.IsActivated()) {
    // Send a commit response message to delilah
    LM_W(("Error commiting to data model in push operation: %s", error.GetMessage().c_str()));
    packet->message->mutable_error()->set_message(au::str("Error: %s", error.GetMessage().c_str()));
  }
  samson_worker_->network()->Send(packet);

  // Flag as commited
  commited_ = true;
}

std::string PushOperation::getStrIdentifiers() const {
  return au::str("%s ( puhs_id %lu )", au::code64_str(delilah_id_).c_str(), push_id_);
}

size_t PushOperation::get_block_id() const {
  return block_id_;
}

std::string PushOperation::getStatus() const {
  if (!distributed_) {
    return "Distributing...";
  }
  if (commited_) {
    return "Comitted";
  }

  return "Distributed. Pending commit...";
}

std::string PushOperation::getStrBufferInfo() const {
  return header.str();
}
}
