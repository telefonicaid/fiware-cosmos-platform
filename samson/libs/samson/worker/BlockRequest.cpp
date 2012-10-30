
#include "samson/stream/BlockManager.h"
#include "samson/worker/SamsonWorker.h"

#include "BlockRequest.h" // Own interface

namespace samson {

  
  BlockRequest::BlockRequest(SamsonWorker *samson_worker, size_t block_id ) {
    
    samson_worker_ = samson_worker;
    block_id_ = block_id;
    loops_ = 0;
    tries_ = 0;

    Reset();
  }
  
  void BlockRequest::Reset()
  {
    // Reset the last cronometer
    last_request_cronometer_.Reset();

    
    // No worker selected so far
    worker_id_ = (size_t)-1;
    
    // If the block manager contains this block, just set as finished
    if( stream::BlockManager::shared()->GetBlock(block_id_) != NULL )
    {
      finished_ = true;
      return;
    }
    
    // If no workers to sent request, get the list of all workers
    if( next_worker_ids_.size() == 0 )
    {
      next_worker_ids_ = samson_worker_->worker_controller()->GetWorkerIds();
      next_worker_ids_.erase( samson_worker_->worker_controller()->worker_id() );      // Remove myself from the list...
      
      if( next_worker_ids_.size() > 0 )
        loops_++;
    }
    
    if( next_worker_ids_.size() == 0 )
      return; // nothing to do if we have no workers to send
    
    // Select a worker
    worker_id_ = worker_from_block_id( block_id_ ); // Try original worker
    if( next_worker_ids_.find(worker_id_) == next_worker_ids_.end() )
      worker_id_ = *next_worker_ids_.begin();
    next_worker_ids_.erase(worker_id_);
    
    // Send packet to selected worker
    PacketPointer packet(new Packet(Message::BlockRequest));
    packet->message->set_block_id(block_id_);
    packet->to = NodeIdentifier(WorkerNode, worker_id_);
    samson_worker_->network()->Send(packet);
    
    tries_++;
  }
  
}