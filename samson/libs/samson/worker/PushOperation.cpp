#include "samson/worker/PushOperation.h"  // Own interface

#include "samson/network/Packet.h"
#include "samson/stream/BlockManager.h"
#include "samson/worker/SamsonWorker.h"
#include "samson/worker/CommitCommand.h"

/*
namespace samson {
PushOperation::PushOperation(SamsonWorker *samson_worker
                             , size_t block_id
                             , size_t block_size
                             , size_t delilah_id
                             , size_t push_id
                             , engine::BufferPointer buffer
                             , const std::vector<std::string>& queues) {
  
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

  // It is not finish by default
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



std::string PushOperation::getStrIdentifiers() const {
  return au::str("%s ( puhs_id %lu )", au::code64_str(delilah_id_).c_str(), push_id_);
}

size_t PushOperation::get_block_id() const {
  return block_id_;
}

std::string PushOperation::getStatus() const {

  if (commited_) {
    return "Comitted";
  }

  return "Received. Pending commit...";
}

std::string PushOperation::getStrBufferInfo() const {
  return header.str();
}
}

*/