#include "samson/worker/WorkerBlockManager.h"  // Own interface

#include <utility>    // std::pair<>

#include "engine/Engine.h"
#include "engine/Notification.h"

#include "samson/worker/SamsonWorker.h"

namespace samson {

void WorkerBlockManager::Review() {
  ReviewDistributionOperations();   // Review distribution blocks and remove finished...
  ReviewPushOperations();   // Review push operations
  ReviewBlockRequests();   // Review all block requests
}

void WorkerBlockManager::ReviewPushOperations() {
  au::set<PushOperation>::iterator it;   // Remove old operations...
  for (it = push_operations_.begin(); it != push_operations_.end(); ++it) {
    PushOperation *push_operation = *it;
    if (push_operation->time() > 60) {
      delete push_operation;
      push_operations_.erase(it);
      return;
    }
  }
}

void WorkerBlockManager::ReviewBlockRequests() {
  // Collection of block ids to request again
  std::vector<size_t> block_ids;

  au::map<size_t, BlockRequest>::iterator it;
  for (it = block_requests_.begin(); it != block_requests_.end(); ++it) {
    if (it->second->creation_time() > 30) {
      block_ids.push_back(it->first);
    }
  }

  // Remove selected requests
  for (size_t i = 0; i < block_ids.size(); ++i) {
    BlockRequest *request = block_requests_.extractFromMap(block_ids[i]);
    if (request) {
      delete request;
    }
  }
}

void WorkerBlockManager::ReviewDistributionOperations() {
  std::vector<size_t> remove_block_ids;
  au::map<size_t, DistributionOperation>::iterator it;
  for (it = distribution_operations_.begin(); it != distribution_operations_.end(); ++it) {
    it->second->Review();
    if (it->second->IsReady()) {
      remove_block_ids.push_back(it->first);
    }
  }

  for (size_t i = 0; i < remove_block_ids.size(); ++i) {
    size_t block_id = remove_block_ids[i];

    // Notify to alert that this block is completelly distributed
    engine::Notification *notification = new engine::Notification("notification_block_correctly_distributed");
    notification->environment().Set("block_id", block_id);
    engine::Engine::shared()->notify(notification);

    // Remove from the map
    DistributionOperation *distribution_block = distribution_operations_.extractFromMap(block_id);
    if (distribution_block) {
      delete distribution_block;
    }
  }
}

au::SharedPointer<gpb::Collection> WorkerBlockManager::GetCollectionForDistributionOperations(
                                                                                              const Visualization& visualization) {
  return GetCollectionForMap("distribution_blocks", distribution_operations_, visualization);
}

au::SharedPointer<gpb::Collection> WorkerBlockManager::GetCollectionForBlockRequests(const Visualization& visualization) {
  return GetCollectionForMap("block_requests", block_requests_, visualization);
}

size_t WorkerBlockManager::CreateBlock(engine::BufferPointer buffer, size_t block_id) {
  // If a block_id is provided, we just include in the local block manager
  if (block_id != (size_t) -1) {
    stream::BlockManager::shared()->CreateBlock(block_id, buffer);
    return block_id;
  }

  // If worker is not connected, it does not make any sense to create a block
  if (!samson_worker_->IsConnected()) {
    return (size_t) -1;
  }

  // Get a new block id
  block_id = samson_worker_->worker_controller()->get_new_block_id();

  // Detect error creating block
  if (block_id == (size_t) -1) {
    return block_id;
  }

  // Set this worker as the creator of this buffer
  KVHeader *header = reinterpret_cast<KVHeader *>(buffer->data());
  header->worker_id = samson_worker_->worker_controller()->worker_id();

  // Insert in the local BlockManager
  stream::BlockManager::shared()->CreateBlock(block_id, buffer);

  // Create a distribution block ( insert in local block manager and distribute to replicas )
  DistributionOperation *distribution_block = new DistributionOperation(samson_worker_, block_id);
  distribution_operations_.insertInMap(block_id, distribution_block);

  return block_id;
}

void WorkerBlockManager::ReceivedBlockDistributionResponse(size_t block_id, size_t worker_id) {
  DistributionOperation *distribution_block = distribution_operations_.findInMap(block_id);

  if (distribution_block) {
    distribution_block->ConfirmWorker(worker_id);
  }
  Review();
}

// Auxiliar function to get list of workers for a block
std::vector<size_t> get_workers_for_block_id(const std::multimap<size_t, size_t>& blocks_map, size_t block_id) {
  std::vector<size_t> worker_ids;
  typedef std::multimap<size_t, size_t>::const_iterator iterator;

  // Get the list of workers containing this block
  std::pair<iterator, iterator> range = blocks_map.equal_range(block_id);

  // Look at returned range
  for (iterator i = range.first; i != range.second; ++i) {
    worker_ids.push_back(i->second);
  }

  return worker_ids;
}

void WorkerBlockManager::RequestBlocks(const std::set<size_t>& pending_block_ids) {
  // Get a complete information of block distribution
  std::multimap<size_t, size_t> blocks_map;
  int rc = samson_worker_->worker_controller()->GetBlockMap(blocks_map);

  if (rc) {
    LM_W(("Not possible to request missing blocks %s", zoo::str_error(rc).c_str()));
    return;
  }

  std::set<size_t>::const_iterator it;
  for (it = pending_block_ids.begin(); it != pending_block_ids.end(); ++it) {
    // Identifier of the block
    size_t block_id = *it;

    // If we have requested this block before, just skip...
    if (block_requests_.findInMap(block_id)) {
      continue;   // Get the list of workers
    }
    std::vector<size_t> worker_ids = get_workers_for_block_id(blocks_map, block_id);

    // Select a random worker
    if (worker_ids.size() == 0) {
      LM_W(("Block %lu is not present in any worker.", block_id));
      continue;
    }

    int w = rand() % worker_ids.size();
    size_t worker_id = worker_ids[w];

    // Add a request to this worker
    BlockRequest *block_request = new BlockRequest(samson_worker_, block_id, worker_id);
    block_requests_.insertInMap(block_id, block_request);

    LM_W(("New block request for block %lu to worker %lu", block_id, worker_id));
  }
}

void WorkerBlockManager::ReceivedBlockDistribution(size_t block_id, size_t worker_id, engine::BufferPointer buffer) {
  // Remove the block request ( if we really requested this block )
  BlockRequest *block_request = block_requests_.extractFromMap(block_id);

  if (block_request) {
    delete block_request;
  }
}

void WorkerBlockManager::Reset() {
  // Remove all internal elements
  distribution_operations_.clearMap();
  block_requests_.clearMap();
  push_operations_.clearSet();
}

// Received a message from a delilah
void WorkerBlockManager::receive_push_block(size_t delilah_id, size_t push_id, engine::BufferPointer buffer,
                                            const std::vector<std::string>& queues) {
  if (buffer == NULL) {
    LM_W(("Push message without a buffer. This is an error..."));
    return;
  }

  KVHeader *header = reinterpret_cast<KVHeader *>(buffer->data());

  if (!header->check() || !header->range.isValid()) {
    LM_W(("Push message with a non-valid buffer.Ignoring..."));
    return;
  }

  if (header->isTxt()) {
    // Modify incoming buffer to assign only one hg
    KVRanges ranges = samson_worker_->worker_controller()->GetMyKVRanges();
    int hg = ranges.RandomHashGroup();
    header->range.set(hg, hg + 1);
  } else if (header->isModule()) {
    // Make sure it is full range
    header->range.set(0, KVFILE_NUM_HASHGROUPS);
  } else {
    LM_W(("Push message with a buffer that is not data or a module.Ignoring..."));
    return;
  }

  // Create a new block in this worker ( and start distribution process )
  size_t block_size = buffer->size();
  size_t block_id = samson_worker_->worker_block_manager()->CreateBlock(buffer);

  if (block_id == (size_t) -1) {
    LM_W(("Error creating block in a push operation ( block_id -1 )"));
    return;
  }

  // Check valid header
  if (buffer->size() < sizeof(KVHeader)) {
    LM_W(("Push message with a non-valid buffer.Ignoring..."));
    return;
  }

  // Create PushOpertion object and insert in the vector of pending push operations
  PushOperation *push_operation = new PushOperation(samson_worker_, block_id, block_size, delilah_id, push_id, buffer,
                                                    queues);
  push_operations_.insert(push_operation);

  LM_TODO(("If the push is ready to confirm distirbution, answer here"));

  return;
}

void WorkerBlockManager::receive_push_block_commit(size_t delilah_id, size_t push_id) {
  // Find the block, run the commit and remove push operation
  au::set<PushOperation>::iterator it;
  for (it = push_operations_.begin(); it != push_operations_.end(); ++it) {
    PushOperation *push_operation = *it;
    if (push_operation->get_delilah_id() == delilah_id) {
      if (push_operation->get_push_id() == push_id) {
        push_operation->commit();
        delete push_operation;
        push_operations_.erase(it);
        return;
      }
    }
  }

  LM_W(("Unused commit message for a push operation"));
  return;
}

au::SharedPointer<gpb::Collection> WorkerBlockManager::GetCollectionForPushOperations(
                                                                                      const Visualization& visualization) {
  au::SharedPointer<gpb::Collection> collection(new gpb::Collection());
  collection->set_name("push operations");

  au::set<PushOperation>::iterator it;
  for (it = push_operations_.begin(); it != push_operations_.end(); ++it) {
    PushOperation *push_operation = *it;

    gpb::CollectionRecord *record = collection->add_record();

    ::samson::add(record, "Delilah", push_operation->getStrIdentifiers());
    ::samson::add(record, "block_id", push_operation->get_block_id());
    ::samson::add(record, "Buffer", push_operation->getStrBufferInfo());
    ::samson::add(record, "Status", push_operation->getStatus());
  }

  return collection;
}

void WorkerBlockManager::notify(engine::Notification *notification) {
  if (!notification->isName(notification_worker_block_manager_review)) {
    return;
  }
}

bool WorkerBlockManager::IsBlockBeingDistributed(size_t block_id) {
  au::map<size_t, DistributionOperation>::iterator it;
  for (it = distribution_operations_.begin(); it != distribution_operations_.end(); ++it)
    if (it->second->block_id() == block_id)
      return true;
  return false;
}
}
