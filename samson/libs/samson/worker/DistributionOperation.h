#ifndef _H_SAMSON_DISTRIBUTION_BLOCKS
#define _H_SAMSON_DISTRIBUTION_BLOCKS

#include <set>
#include <string>

#include "au/containers/Uint64Vector.h"
#include "au/statistics/Cronometer.h"

#include "samson/common/KVHeader.h"
#include "samson/common/MessagesOperations.h"
#include "samson/common/samson.pb.h"


// NOTE: Distribution operation are not necessary any more with the new pull-based stream processing architecture
/*
namespace samson {
class SamsonWorker;

// Blocks newly created in the distribution process

class DistributionOperation {
  public:
    DistributionOperation(SamsonWorker *samsonWorker, size_t block_id);
    ~DistributionOperation() {
    }

    // General review function of this distribution block
    void Review();

    // Receive the confirmation that this block has been confirmed by a worker
    void ConfirmWorker(size_t worker_id);

    // Check if al the workers are notified
    bool IsReady();

    // Get information for delilah listing
    void fill(gpb::CollectionRecord *record, const Visualization& visualization);

    // Accessorts
    size_t block_id() const {
      return block_id_;
    }

  private:
    SamsonWorker *samson_worker_;   // Pointer to samsonWorker to interact with network, worker_controller,...
    size_t block_id_;   // Identifier of this block

    au::Uint64Set worker_ids_;   // Workers selected to have duplicates of this block
    au::Uint64Set sent_duplicate_worker_ids_;   // workers we have sent this message
    au::Uint64Set confirmed_worker_ids_;   // Workers confirmed to have received this block

    au::Cronometer cronometer_;   // Cronometer to reset operations
};
}

 */
#endif  // ifndef _H_SAMSON_DISTRIBUTION_BLOCKS
