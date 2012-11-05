#ifndef WORKER_BLOCK_MANAGER_H_
#define WORKER_BLOCK_MANAGER_H_

#include <map>
#include <set>
#include <string>
#include <vector>

#include "au/containers/map.h"
#include "au/containers/set.h"
#include "au/containers/SharedPointer.h"

#include "engine/Buffer.h"
#include "engine/NotificationListener.h"

#include "samson/common/Visualitzation.h"
#include "samson/common/samson.pb.h"

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
  
  class WorkerBlockManager {
  public:
    
    WorkerBlockManager(SamsonWorker *samson_worker) {
      samson_worker_ = samson_worker;
    }
    ~WorkerBlockManager() {}
    
    // Create a block adding to the block manager ( output of any operation executed in this worker )
    size_t CreateBlock( engine::BufferPointer buffer );
    
    // Notify that a block request response message has been received
    void ReceivedBlockRequestResponse(size_t block_id, size_t worker_id , bool error);

    // Add block requests
    void RequestBlocks(const std::set<size_t>& pending_block_ids);
    void RequestBlock(size_t block_id);
    
    // Review all kind of elements
    void Review();
    
    // Receive a push block from delilah
    void ReceivedPushBlock(  size_t delilah_id
                           , size_t push_id
                           , engine::BufferPointer buffer
                           , const std::vector<std::string>& queues );
    
    // General reset command ( worker has disconnected )
    void Reset();
    
    // Collections for all internal elements
    au::SharedPointer<gpb::Collection> GetCollectionForBlockRequests(const Visualization& visualization);
    
  private:
    
    SamsonWorker *samson_worker_;
    au::map<size_t, BlockRequest> block_requests_;   // Block requests sent by this worker
  };
  
}

#endif   // ifndef WORKER_BLOCK_MANAGER_H_
