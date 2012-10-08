#ifndef _H_SAMSON_BLOCK_REQUEST
#define _H_SAMSON_BLOCK_REQUEST

#include "au/Cronometer.h"
#include "au/containers/Uint64Vector.h"
#include "samson/common/KVHeader.h"
#include "samson/common/MessagesOperations.h"
#include "samson/common/samson.pb.h"
#include <set>
#include <string>


namespace samson {
  class SamsonWorker;
  
  class BlockRequest {
  public:
    
    BlockRequest(SamsonWorker *samson_worker, size_t block_id, size_t worker_id);
    ~BlockRequest() {
    }
    
    size_t creation_time() {
      return cronometer_.seconds();
    }
    
    // Get information for delilah listing
    void fill(gpb::CollectionRecord *record, const Visualization& visualization) {
      add(record, "block_id", block_id_, "different");
      add(record, "target_worker_id", worker_id_, "different");
      add(record, "time", cronometer_.str(), "different");
    }
    
  private:
    
    SamsonWorker *samson_worker_;      // Pointer to samsonWorker to interact with network, worker_controller,...
    size_t block_id_;                  // Identifier of this block
    size_t worker_id_;                 // Worker where we have sent this data
    au::Cronometer cronometer_;        // Cronometer since creation ( to reset )
  };
}

#endif