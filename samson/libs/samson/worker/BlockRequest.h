#ifndef _H_SAMSON_BLOCK_REQUEST
#define _H_SAMSON_BLOCK_REQUEST

#include "au/statistics/Cronometer.h"
#include "au/containers/Uint64Vector.h"
#include "samson/common/KVHeader.h"
#include "samson/stream/Block.h"
#include "samson/common/MessagesOperations.h"
#include "samson/common/samson.pb.h"
#include <set>
#include <string>


namespace samson {
  class SamsonWorker;
  
  class BlockRequest {
  public:
    
    BlockRequest(SamsonWorker *samson_worker, size_t block_id);
    ~BlockRequest() {
    }
    
    size_t creation_time() {
      return cronometer_.seconds();
    }

    void Review()
    {
      // If too much time waiting for an answer request to another worker
      // If worker is dead, we also chose another worker
      if( cronometer_.seconds() > 30 )
        Reset();
    }
    
    void GotErrorMessage( size_t worker_id )
    {
      if( worker_id != worker_id_ )
        return; // Old request
      Reset();
      
    }
    
    bool finished()
    {
      return finished_;
    }
    
    
    // Get information for delilah listing
    void fill(gpb::CollectionRecord *record, const Visualization& visualization) {
      add(record, "block_id", str_block_id(block_id_), "different");
      add(record, "target_worker_id", worker_id_, "different");
      add(record, "tries", tries_, "uint64,different");
      add(record, "lopps", loops_, "uint64,different");
      add(record, "time", cronometer_.str(), "different");
      add(record, "last request time", last_request_cronometer_.str(), "different");
    }
    
  private:
    
    void Reset();
    
    SamsonWorker *samson_worker_;      // Pointer to samsonWorker to interact with network, worker_controller,...
    int tries_;                        // Number of workers requested
    int loops_;                        // Number of times we have loop SAMSON cluster for this block
    size_t block_id_;                  // Identifier of this block
    size_t worker_id_;                 // Worker where we have sent this data
    au::Cronometer cronometer_;        // Cronometer since creation ( to reset )
    au::Cronometer last_request_cronometer_;
    
    bool finished_;                    // Flag to indicate workers
    std::set<size_t> next_worker_ids_; // List of workers id to test

  };
}

#endif