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
#ifndef WORKER_BLOCK_MANAGER_H_
#define WORKER_BLOCK_MANAGER_H_

#include <map>
#include <set>
#include <string>
#include <vector>

#include "au/containers/SharedPointer.h"
#include "au/containers/map.h"
#include "au/containers/set.h"

#include "engine/Buffer.h"
#include "engine/NotificationListener.h"

#include "samson/common/Logs.h"
#include "samson/common/Visualitzation.h"


#include "samson/worker/BlockRequest.h"

/*
 *
 * WorkerBlockManager
 *
 * Manager for all block related operations in workers
 *
 * Push operations from delilahs
 * Distributions between workers
 * Block requests between workers
 *
 *
 */

namespace samson {
class SamsonWorker;
namespace stream {
class DefragTask;
}

/**
 * \brief Class to keep information of a push operation from a delilah pending to be confirmed
 */

class WorkerPushItem {
public:
  WorkerPushItem(size_t delilah_id, size_t push_id, size_t commit_id) :
    delilah_id_(delilah_id), push_id_(push_id), commit_id_(commit_id) {
  }

  ~WorkerPushItem() {
  }

  size_t push_id() const {
    return push_id_;
  }

  size_t commit_id() const {
    return commit_id_;
  }

  size_t delilah_id() const {
    return delilah_id_;
  }

private:
  size_t delilah_id_;
  size_t push_id_;
  size_t commit_id_;
};

class WorkerBlockManager {
public:

  WorkerBlockManager(SamsonWorker *samson_worker) {
    samson_worker_ = samson_worker;
  }

  ~WorkerBlockManager() {
  }

  /*
   * \brief Create a block adding to the block manager
   * Output of any operation in this worker create blocks using this method
   */
  size_t CreateBlock(engine::BufferPointer buffer);


  /**
   * \brief Add A new block request
   */

  void RequestBlocks(const std::set<size_t>& pending_block_ids);
  void RequestBlock(size_t block_id);

  /*
   * \brief Notify that a block request response message has been received
   */

  void ReceivedBlockRequestResponse(size_t block_id, size_t worker_id);

  /*
   * \brief Notify that a block request response message has been received with an error
   */

  void ReceivedBlockRequestResponse(size_t block_id, size_t worker_id, const std::string& error_message);

  // Add a block-break request
  void AddBlockBreak(const std::string& queue_name, size_t block_id, const std::vector<KVRange>& ranges);

  // Review all kind of elements
  void Review();

  /**
   * \brief Review push items based on consolidated commit_id and current commit_id
   */
  void ReviewPushItems(size_t previous_data_commit_id, size_t current_data_commit_id);

  /**
   * \brief Notify that a push has been received
   */

  void ReceivedPushBlock(size_t delilah_id
                         , size_t push_id
                         , engine::BufferPointer buffer
                         , const std::vector<std::string>& queues);

  // General reset command (i.e. when worker is disconnected )
  void Reset();

  // Remove request for all blocks not belonmging to data model any more
  void RemoveRequestIfNecessary(const std::set<size_t>& all_block_ids);

  // Collections for all internal elements
  au::SharedPointer<gpb::Collection> GetCollectionForBlockRequests(const Visualization& visualization);
  au::SharedPointer<gpb::Collection> GetCollectionForBlockDefrags(const Visualization& visualization);

private:

  void SendPushBlockResponse(size_t delilah_id, size_t push_id);
  void SendPushBlockResponseWithError(size_t delilah_id, size_t push_id, const std::string& error);
  void SendPushBlockConfirmation(size_t delilah_id, size_t push_id);
  void SendPushBlockConfirmationWithError(size_t delilah_id, size_t push_id, const std::string& error);

  SamsonWorker *samson_worker_;
  au::map<size_t, BlockRequest> block_requests_;     // Block requests sent by this worker
  au::Dictionary<std::string, stream::DefragTask > defrag_tasks_;
  std::list<WorkerPushItem> push_items_;  // List of push items pending to be confirmed
};
}

#endif   // ifndef WORKER_BLOCK_MANAGER_H_
