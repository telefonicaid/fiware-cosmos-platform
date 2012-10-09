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

#include "samson/worker/DistributionOperation.h"
#include "samson/worker/BlockRequest.h"
#include "samson/worker/PushOperation.h"

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

class WorkerBlockManager : public engine::NotificationListener {
  public:
    WorkerBlockManager(SamsonWorker *samson_worker) {
      samson_worker_ = samson_worker;
    }
    ~WorkerBlockManager() {
    }

    // Notifications (engine::NotificationListener)
    virtual void notify(engine::Notification *notification);

    // Review all kind of elements
    void Review();

    // Create a new block ( if block_id is provided, it is not distributed, just introduced in stream::BlockManager )
    size_t CreateBlock(engine::BufferPointer buffer, size_t block_id = (size_t) -1);

    // Messages received from other workers
    void ReceivedBlockDistributionResponse(size_t block_id, size_t worker_id);
    void ReceivedBlockDistribution(size_t block_id, size_t worker_id, engine::BufferPointer buffer);

    // Request blocks from other workers
    void RequestBlocks(const std::set<size_t>& pending_block_ids);

    // Receive a push block from delilah
    void receive_push_block(size_t delilah_id, size_t push_id, engine::BufferPointer buffer,
                            const std::vector<std::string>& queues);

    // Recieve the commit command from delilah
    void receive_push_block_commit(size_t delilah_id, size_t push_id);

    // General reset command ( worker has disconnected )
    void Reset();

    // Collections for all internal elements
    au::SharedPointer<gpb::Collection> GetCollectionForDistributionOperations(const Visualization& visualization);
    au::SharedPointer<gpb::Collection> GetCollectionForBlockRequests(const Visualization& visualization);
    au::SharedPointer<gpb::Collection> GetCollectionForPushOperations(const Visualization& visualization);

    // Check if a particular block is being distributed
    // Crapy solution: check if it is not included in any distribution mechanism
    bool IsBlockBeingDistributed(size_t block_id);

  private:

    void ReviewDistributionOperations();
    void ReviewPushOperations();
    void ReviewBlockRequests();

    SamsonWorker *samson_worker_;
    au::map<size_t, DistributionOperation> distribution_operations_;   // Current blocks begin distributed
    au::map<size_t, BlockRequest> block_requests_;   // Block requests sent by this worker
    au::set<PushOperation> push_operations_;   // Push operations from delilahs
};
}

#endif   // ifndef WORKER_BLOCK_MANAGER_H_
