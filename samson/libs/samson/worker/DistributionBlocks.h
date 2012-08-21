#ifndef _H_SAMSON_DISTRIBUTION_BLOCKS
#define _H_SAMSON_DISTRIBUTION_BLOCKS

#include "au/containers/Uint64Vector.h"
#include "samson/common/samson.pb.h"
#include <set>
#include <string>


namespace samson {
class SamsonWorker;

// Blocks newly created in the distribution process

class DistributionBlock {
public:

  DistributionBlock(SamsonWorker *samsonWorker, size_t block_id, engine::BufferPointer buffer);
  ~DistributionBlock() {
  }

  // Revirew if we have sent this block to all required workers
  void review();

  // Receive the confirmation that this block has been confirmed by a worker
  void confirm_worker(size_t worker_id);

  // Check if al the workers are notified
  bool isReady();

  // Get information for delilah listing
  void fill(gpb::CollectionRecord *record, const Visualization& visualization) {
    add(record, "block_id", block_id_, "different");
    add(record, "workers", worker_ids_.str(), "different");
    add(record, "confirmed workers", confirmed_worker_ids_.str(), "different");

    add(record, "size", buffer_->getSize(), "different");
    add(record, "block", header.str(), "different");
  }

private:

  SamsonWorker *samsonWorker_;              // Pointer to samsonWorker to interact with network, worker_controller,...
  size_t block_id_;                         // Identifier of this block
  au::Uint64Set worker_ids_;                // Workers selected to recepice duplicate blocks
  au::Uint64Set confirmed_worker_ids_;      // Workers  confirmed to have received this block

  engine::BufferPointer buffer_;            // Retain in memory while distributing

  // Copy of the header
  KVHeader header;
};


class BlockRequest {
public:

  BlockRequest(SamsonWorker *samson_worker, size_t block_id, size_t worker_id);
  ~BlockRequest() {
  }

  // Get information for delilah listing
  void fill(gpb::CollectionRecord *record, const Visualization& visualization) {
    add(record, "block_id", block_id_, "different");
    add(record, "worker_id", worker_id_, "different");
  }

private:

  SamsonWorker *samson_worker_;      // Pointer to samsonWorker to interact with network, worker_controller,...
  size_t block_id_;                  // Identifier of this block
  size_t worker_id_;                 // Worker where we have sent this data
};

class DistributionBlockManager {
public:

  DistributionBlockManager(SamsonWorker *samson_worker) {
    samson_worker_ = samson_worker;
  };

  ~DistributionBlockManager() {
  };

  // Review scheduled distributions
  void Review();

  // Create a new block ( if id is provided, it is not distributed )
  size_t CreateBlock(engine::BufferPointer buffer, size_t block_id = (size_t)-1);

  // Request blocks from other workers
  void RequestBlocks(const std::set<size_t>& pending_block_ids);

  // Confirm a new block
  void ConfirmBlockDistribution(size_t block_id, size_t worker_id);

  // Collections for distribution and requests block operations
  gpb::Collection *GetCollectionForDistributionBlocks(const Visualization& visualization) {
    return getCollectionForMap("distribution_blocks", distribution_blocks_, visualization);
  }

  gpb::Collection *GetCollectionForBlockRequests(const Visualization& visualization) {
    return getCollectionForMap("block_requests", block_requests_, visualization);
  }

private:

  SamsonWorker *samson_worker_;
  au::map<size_t, DistributionBlock> distribution_blocks_;
  au::map<size_t, BlockRequest> block_requests_;
};
}

#endif  // ifndef _H_SAMSON_DISTRIBUTION_BLOCKS