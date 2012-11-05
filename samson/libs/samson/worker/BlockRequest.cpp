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

#include "samson/stream/BlockManager.h"
#include "samson/worker/SamsonWorker.h"
#include "samson/common/Logs.h"

#include "BlockRequest.h" // Own interface

namespace samson {

  
  BlockRequest::BlockRequest(SamsonWorker *samson_worker, size_t block_id ) {

    AU_M(logs.block_request, ("New block request %s" , str_block_id( block_id ).c_str() ));
    
    samson_worker_ = samson_worker;
    block_id_ = block_id;
    loops_ = 0;
    tries_ = 0;

    Reset();
  }
  
  void BlockRequest::Reset()
  {
    AU_D(logs.block_request, ("[%s] Reset" , str_block_id( block_id_ ).c_str() ));
    
    // Reset the last cronometer
    last_request_cronometer_.Reset();

    
    // No worker selected so far
    worker_id_ = (size_t)-1;
    
    // If the block manager contains this block, just set as finished
    if( stream::BlockManager::shared()->GetBlock(block_id_) != NULL )
    {
      AU_D(logs.block_request, ("[%s] Found in local block manager" , str_block_id( block_id_ ).c_str() ));
      finished_ = true;
      return;
    }
    
    // If no workers to sent request, get the list of all workers
    if( next_worker_ids_.size() == 0 )
    {
      
      next_worker_ids_ = samson_worker_->worker_controller()->GetWorkerIds();
      next_worker_ids_.erase( samson_worker_->worker_controller()->worker_id() );      // Remove myself from the list...

      AU_D(logs.block_request, ("[%s] Getting list of workers.... %lu workers"
                                , str_block_id( block_id_ ).c_str()
                                , next_worker_ids_.size() ));
      
      if( next_worker_ids_.size() > 0 )
        loops_++;
    }
    
    if( next_worker_ids_.size() == 0 )
    {
      AU_D(logs.block_request, ("[%s] No workers to find this block" , str_block_id( block_id_ ).c_str() ));
      return; // nothing to do if we have no workers to send
    }
    
    // Select a worker
    worker_id_ = worker_from_block_id( block_id_ ); // Try original worker
    if( next_worker_ids_.find(worker_id_) == next_worker_ids_.end() )
      worker_id_ = *next_worker_ids_.begin();
    next_worker_ids_.erase(worker_id_);
    
    // Send packet to selected worker
    AU_D(logs.block_request, ("[%s] Send request to worker %lu" , str_block_id( block_id_ ).c_str() , worker_id_ ));
    PacketPointer packet(new Packet(Message::BlockRequest));
    packet->message->set_block_id(block_id_);
    packet->to = NodeIdentifier(WorkerNode, worker_id_);
    samson_worker_->network()->Send(packet);
    
    tries_++;
  }
  
}