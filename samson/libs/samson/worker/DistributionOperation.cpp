#include "samson/worker/DistributionOperation.h"  // Own interface

#include <vector>

#include "samson/stream/BlockManager.h"
#include "samson/worker/SamsonWorker.h"

namespace samson {
DistributionOperation::DistributionOperation(SamsonWorker *samson_worker, size_t block_id) {
  samson_worker_ = samson_worker;   // keep a pointer to samson worker
  block_id_ = block_id;   // Keep the block_id

  // First review to include necessary workers
  Review();
}

void DistributionOperation::ConfirmWorker(size_t worker_id) {
  if (worker_ids_.contains(worker_id)) {
    confirmed_worker_ids_.insert(worker_id);
  } else {
    LM_W(("Worker %lu confirmed on block %lu. It was not a selected worker...", worker_id, block_id_));
  }
}

void DistributionOperation::Review() {
  // Timeout for the rest of workers
  if (cronometer_.seconds() > 30) {
    cronometer_.Reset();
    // We onlu consider sent, workers that have confirmed reception
    sent_duplicate_worker_ids_ = confirmed_worker_ids_;
  }

  // Recover block
  stream::BlockPointer block = stream::BlockManager::shared()->GetBlock(block_id_);
  if (block == NULL) {
    worker_ids_.clear();   // This block does no exist any more, just do not distribute
    return;
  }

  // Get the list of workers where this block should be replicated
  worker_ids_ = samson_worker_->worker_controller()->GetWorkerIdsForRange(block->getKVRange());

  // Check if we have to sent the packet to new workers
  std::vector<size_t> new_worker_ids;
  std::set<size_t>::iterator it;
  for (it = worker_ids_.begin(); it != worker_ids_.end(); it++) {
    size_t worker_id = *it;
    if (sent_duplicate_worker_ids_.contains(worker_id)) {
      continue;
    }
    new_worker_ids.push_back(worker_id);
    sent_duplicate_worker_ids_.insert(worker_id);
  }
  // Add a task to sent this block to all new workers
  if (new_worker_ids.size() > 0) {
    samson_worker_->task_manager()->AddBlockDistributionTask(block_id_, new_worker_ids);
  }
}

void DistributionOperation::fill(gpb::CollectionRecord *record, const Visualization& visualization) {
  add(record, "block_id", block_id_, "different");
  add(record, "workers", worker_ids_.str(), "different");
  add(record, "confirmed workers", confirmed_worker_ids_.str(), "different");
  add(record, "time", cronometer_.str(), "different");
}

bool DistributionOperation::IsReady() {
  std::set<size_t>::iterator it;   // Check all selected workers have a copy...
  for (it = worker_ids_.begin(); it != worker_ids_.end(); it++) {
    if (!confirmed_worker_ids_.contains(*it)) {
      return false;
    }
  }
  return true;
}
}
