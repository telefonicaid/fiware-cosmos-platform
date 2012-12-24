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

#include "samson/worker/BlockRequest.h"  // Own interface

#include "au/string/StringUtilities.h"
#include "samson/common/Logs.h"
#include "samson/stream/BlockManager.h"
#include "samson/worker/SamsonWorker.h"

namespace samson {
size_t pick_random(std::set<size_t>& elements) {
  size_t size = elements.size();

  if (size == 0) {
    LM_X(1, ("Internal error"));
  }
  int pos = rand() % elements.size();
  std::set<size_t>::iterator iter = elements.begin();
  for (int p = 0; p < pos; p++) {
    iter++;
  }
  size_t value = *iter;
  elements.erase(iter);
  if (elements.size() != (size - 1)) {
    LM_X(1, ("Internal error"));
  }
  return value;
};

void BlockRequest::NotifyErrorMessage(size_t worker_id, const std::string& error_message) {
  if (worker_id != worker_id_) {
    LOG_D(logs.block_request, ("[%s] Received a block request from an unexpected worker (%lu != %lu). Ignoring..."
                               , str_block_id(block_id_).c_str()
                               , worker_id
                               , worker_id_));
    return;
  }

  // Some debug information for lists
  fails_.Add(au::str("W%lu", worker_id));

  // Remove this worker form the list of candidates
  if ((worker_ids_.size() > 0) && (worker_ids_.front() == worker_id)) {
    worker_ids_.pop_front();
  }
  worker_id_ = SIZE_T_UNDEFINED;    // Mark as "unsent"
  Review();     // Review this connection
}

void BlockRequest::fill(gpb::CollectionRecord *record, const Visualization& visualization) {
  add(record, "block_id", str_block_id(block_id_), "different");
  add(record, "time", cronometer_.str(), "different");
  add(record, "finished", finished_ ? "yes" : "no", "different");

  add(record, "Target workers", au::str(worker_ids_), "different");

  add(record, "last request time", last_request_cronometer_.str(), "different");
  add(record, "sent", sent_packets_.str(), "different");
  add(record, "fails", fails_.str(), "different");
  add(record, "state", state_, "different");
}

BlockRequest::BlockRequest(SamsonWorker *samson_worker, size_t block_id)
  : samson_worker_(samson_worker)
    , block_id_(block_id)
    , worker_id_(SIZE_T_UNDEFINED)
    , finished_(false)
    , state_("Created") {
  LOG_V(logs.block_request, ("New block request %s", str_block_id(block_id).c_str()));

  Review();
}

void BlockRequest::Review() {
  if (finished_) {
    state_ = "Finished";
    return;   // Already done
  }

  // If the block manager contains this block, just set as finished
  if (stream::BlockManager::shared()->Contains(block_id_)) {
    LOG_V(logs.block_request, ("[%s] Found in local block manager", str_block_id(block_id_).c_str()));
    finished_ = true;
    state_ = "Finished";
    return;
  }

  LOG_D(logs.block_request, ("[%s] Review", str_block_id(block_id_).c_str()));


  if (worker_id_ != SIZE_T_UNDEFINED) {
    if (last_request_cronometer_.seconds() < 30) {
      state_ = au::str("Waiting response from worker %lu (%s)", worker_id_, last_request_cronometer_.str().c_str());
      return;
    }
    // Remove this worker as a candidate for timeout
    if ((worker_ids_.size() > 0) && (worker_ids_.front() == worker_id_)) {
      worker_ids_.pop_front();
    }
    fails_.Add(au::str("W%lu-timeout", worker_id_));    // Some debug information for lists
    worker_id_ = SIZE_T_UNDEFINED;
  }

  // If no workers to sent request, get the list of all workers
  if (worker_ids_.size() == 0) {
    // Get all worker ids
    std::set<size_t> worker_ids = samson_worker_->worker_controller()->GetWorkerIds();
    worker_ids.erase(samson_worker_->worker_controller()->worker_id());     // Remove myself from the list...

    LOG_D(logs.block_request, ("[%s] Getting list of workers.... %lu workers"
                               , str_block_id(block_id_).c_str()
                               , worker_ids.size()));

    // If still part of the cluster, try with the original
    size_t original_worker_id = worker_from_block_id(block_id_);
    if (worker_ids.find(original_worker_id) != worker_ids.end()) {
      worker_ids.erase(original_worker_id);
      worker_ids_.push_back(original_worker_id);
    }

    // Add the rest of workers in random order
    while (worker_ids.size() > 0) {
      size_t worker_id = pick_random(worker_ids);
      worker_ids_.push_back(worker_id);
    }
  }

  if (worker_ids_.size() == 0) {
    LOG_D(logs.block_request, ("[%s] No workers to find this block", str_block_id(block_id_).c_str()));
    state_ = "Empty list of workers to sent requests";
    return;
  }

  // Searh the next worker_id
  size_t worker_id = worker_ids_.front();  // Next worker to test
  if (!samson_worker_->worker_alert().CheckWorker(worker_id)) {
    state_ = au::str("Waiting worker %lu to be ready ( not overloaded ) ", worker_id);
    return;
  }

  // Send request to thist worker
  LOG_V(logs.block_request, ("[%s] Send request to worker %lu", str_block_id(block_id_).c_str(), worker_id));

  worker_id_ = worker_id;
  last_request_cronometer_.Reset();  // Reset the last cronometer
  PacketPointer packet(new Packet(Message::BlockRequest));
  packet->message->set_block_id(block_id_);
  packet->to = NodeIdentifier(WorkerNode, worker_id_);
  samson_worker_->network()->Send(packet);
  state_ = au::str("Waiting response from worker %lu (%s)", worker_id_, last_request_cronometer_.str().c_str());
  sent_packets_.Add(au::str("W%lu", worker_id_));  // Some debug information
}
}
