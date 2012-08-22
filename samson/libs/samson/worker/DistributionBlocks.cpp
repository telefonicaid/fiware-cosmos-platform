


#include "samson/stream/BlockManager.h"
#include "samson/worker/SamsonWorker.h"

#include "DistributionBlocks.h"  // Own interface

namespace samson {
DistributionBlock::DistributionBlock(SamsonWorker *samsonWorker, size_t block_id, engine::BufferPointer buffer) {
  // keep a pointer to samson worker
  samsonWorker_ = samsonWorker;

  // Keep the block_id
  block_id_ = block_id;

  // Keep this buffer in memory just in case we have to re-sent to other workers
  buffer_ = buffer;

  // Get a copy of the header
  memcpy(&header, buffer->getData(), sizeof(KVHeader));

  // First review to include necessary workers
  review();
}

void DistributionBlock::confirm_worker(size_t worker_id) {
  if (worker_ids_.contains(worker_id)) {
    confirmed_worker_ids_.insert(worker_id);
  } else {
    LM_W(("Worker %lu confirmed on block %lu. It was not a selected worker...", worker_id, block_id_ ));
  }
}

void DistributionBlock::review() {
  // Get the list of workers where this block should be replicated
  au::Uint64Set worker_ids = samsonWorker_->worker_controller->GetWorkerIdsForRange(header.range);

  au::Uint64Vector non_included_worker_ids = worker_ids.non_intersection(worker_ids_).getVector();


  // Include workers not previously included
  for (size_t i = 0; i < non_included_worker_ids.size(); i++) {
    // Consider this worker_id
    size_t worker_id = non_included_worker_ids[i];

    // Not previously included
    PacketPointer packet(new Packet(Message::DuplicateBlock));
    packet->message->set_block_id(block_id_);
    packet->set_buffer(buffer_);
    packet->to = NodeIdentifier(WorkerNode, non_included_worker_ids[i]);

    // Send packet
    samsonWorker_->network()->Send(packet);

    // Include in the list of workers
    worker_ids_.insert(worker_id);
  }

  // Remove all blocks not included
  worker_ids.remove_non_included_in(worker_ids);
  confirmed_worker_ids_.remove_non_included_in(worker_ids);
}

bool DistributionBlock::isReady() {
  bool rc = ( confirmed_worker_ids_.size() == worker_ids_.size());

  /*
   * if( rc )
   * LM_W(( "Block %lu is completelly distributed to %lu workers ", block_id_ , worker_ids_.size() ));
   */
  return rc;
}

// ---------

BlockRequest::BlockRequest(SamsonWorker *samson_worker, size_t block_id, size_t worker_id) {
  samson_worker_ = samson_worker;
  block_id_ = block_id;
  worker_id_ = worker_id;


  // Send packet to selected worker
  PacketPointer packet(new Packet(Message::BlockRequest));
  packet->message->set_block_id(block_id_);
  packet->to = NodeIdentifier(WorkerNode, worker_id_);

  // Send packet
  samson_worker_->network()->Send(packet);
}

// ---------

void DistributionBlockManager::Review() {
  std::vector<size_t> remove_block_ids;

  // Review distribution blocks
  au::map<size_t, DistributionBlock>::iterator it;
  for (it = distribution_blocks_.begin(); it != distribution_blocks_.end(); it++) {
    if (it->second->isReady()) {
      remove_block_ids.push_back(it->first);
    }
  }

  for (size_t i = 0; i < remove_block_ids.size(); i++) {
    size_t block_id = remove_block_ids[i];

    // Notify to alert that this block is completelly distributed
    engine::Notification *notification = new engine::Notification("notification_block_correctly_distributed");
    notification->environment().Set("block_id", block_id);
    engine::Engine::shared()->notify(notification);

    // Remove from the map
    DistributionBlock *distribution_block = distribution_blocks_.extractFromMap(block_id);
    if (distribution_block) {
      delete distribution_block;
    }
  }
}

size_t DistributionBlockManager::CreateBlock(engine::BufferPointer buffer, size_t block_id) {
  // If a block_id is provided, we just include in the local block manager
  if (block_id != (size_t)-1) {
    // Insert in the local BlockManager
    stream::BlockManager::shared()->create_block(block_id, buffer);
    return block_id;
  }

  // Get a new block id
  block_id = samson_worker_->get_new_block_id();

  // Insert in the local BlockManager
  stream::BlockManager::shared()->create_block(block_id, buffer);

  // Create a distribution block ( insert in local block manager and distribute to replicas )
  DistributionBlock *distribution_block = new DistributionBlock(samson_worker_, block_id, buffer);
  distribution_blocks_.insertInMap(block_id, distribution_block);

  return block_id;
}

void DistributionBlockManager::ConfirmBlockDistribution(size_t block_id, size_t worker_id) {
  DistributionBlock *distribution_block = distribution_blocks_.findInMap(block_id);

  if (!distribution_block) {
    LM_W(("Message Message::DuplicateBlockResponse for block %lu recevied from worker %lu. Ignored... "
          , worker_id
          , block_id
          ));
    return;
  }

  distribution_block->confirm_worker(worker_id);
  Review();
}

// Auxiliar function to get list of workers for a block
std::vector<size_t> get_workers_for_block_id(const std::multimap<size_t, size_t>& blocks_map, size_t block_id) {
  std::vector<size_t> worker_ids;
  typedef std::multimap<size_t, size_t>::const_iterator   iterator;

  // Get the list of workers containing this block
  std::pair<iterator, iterator> range = blocks_map.equal_range(block_id);

  // Look at returned range
  for (iterator i = range.first; i != range.second; i++) {
    worker_ids.push_back(i->second);
  }

  return worker_ids;
}

void DistributionBlockManager::RequestBlocks(const std::set<size_t>& pending_block_ids) {
  // Get a complete information of block distribution
  std::multimap<size_t, size_t> blocks_map;
  int rc = samson_worker_->worker_controller->GetBlockMap(blocks_map);

  if (rc) {
    LM_W(("Not possible to request missing blocks %s", zoo::str_error(rc).c_str()));
    return;
  }

  std::set<size_t>::const_iterator it;
  for (it = pending_block_ids.begin(); it != pending_block_ids.end(); it++) {
    // Identifier of the block
    size_t block_id = *it;

    // If we have schedules this request skip
    if (block_requests_.findInMap(block_id)) {
      continue;  // Get the list of workers
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

    LM_W(("New block request for block %lu to worker %lu", block_id, worker_id ));
  }
}
}