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
#ifndef _H_SAMSON_BLOCK_REQUEST
#define _H_SAMSON_BLOCK_REQUEST

#include "au/statistics/Cronometer.h"
#include "au/string/Descriptors.h"
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
      if( last_request_cronometer_.seconds() > 30 )
        Reset();
    }
    
    void NotifyErrorMessage( size_t worker_id )
    {
      if( worker_id != worker_id_ )
        return; // Old request response... ignore
      
      fails_.Add( au::str("W%lu" , worker_id ) );
      
      Reset(); // Reset to the next worker
    }
    
    bool finished()
    {
      return finished_;
    }
    
    size_t block_id(){
      return block_id_;
    }
    
    // Get information for delilah listing
    void fill(gpb::CollectionRecord *record, const Visualization& visualization) {
      add(record, "block_id", str_block_id(block_id_), "different");
      add(record, "time", cronometer_.str(), "different");

      add(record, "finished", finished_?"yes":"no", "different");
      
      if( worker_id_ == (size_t) - 1 )
        add(record, "last target worker_id", "-", "different");
      else
        add(record, "last target worker_id", au::str("%lu",worker_id_) , "different");
      
      add(record, "last request time", last_request_cronometer_.str(), "different");
      
      add(record, "sent", sent_packets_.str(), "different");
      add(record, "fails", fails_.str() , "different");
    }
    
  private:
    
    void Reset();
    
    SamsonWorker *samson_worker_;      // Pointer to samsonWorker to interact with network, worker_controller,...

    size_t block_id_;                  // Identifier of this block
    size_t worker_id_;                 // Worker where we have sent this data
    au::Cronometer cronometer_;        // Cronometer since creation ( to reset )
    au::Cronometer last_request_cronometer_;
    
    au::Descriptors sent_packets_;     // Statistics about workers asked for this packet
    au::Descriptors fails_;            // Statistics about workers asked for this packet
    
    bool finished_;                    // Flag to indicate workers
    std::set<size_t> next_worker_ids_; // List of workers id to test

  };
}

#endif