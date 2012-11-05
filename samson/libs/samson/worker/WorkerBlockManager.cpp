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
#include "samson/worker/WorkerBlockManager.h"  // Own interface

#include <utility>    // std::pair<>

#include "au/string/StringUtilities.h"
#include "engine/Engine.h"
#include "engine/Notification.h"
#include "samson/worker/SamsonWorker.h"
#include "samson/common/Logs.h"

namespace samson {
  
  void WorkerBlockManager::Review() {
    
    AU_D(logs.worker_block_manager, ("Review"));
    
    // Review all block requests
    {
      au::map<size_t, BlockRequest>::iterator it;
      for (it = block_requests_.begin(); it != block_requests_.end(); )
      {
        it->second->Review();
        
        // Remove if necessary...
        if( it->second->finished() )
          block_requests_.erase(it++);
        else
          ++it;
      }
    }
  }
  
  size_t WorkerBlockManager::CreateBlock( engine::BufferPointer buffer )
  {
    
    size_t block_id = samson_worker_->worker_controller()->get_new_block_id();
    stream::BlockManager::shared()->CreateBlock(block_id, buffer);
    
    AU_M(logs.worker_block_manager, ("Create block from buffer %s --> %s"
                                     , buffer->str().c_str()
                                     , str_block_id(block_id).c_str() ));
    
    return block_id;
  }
  
  // Messages received from other workers
  void WorkerBlockManager::ReceivedBlockRequestResponse(size_t block_id, size_t worker_id , bool error)
  {
    AU_M(logs.worker_block_manager, ("ReceivedBlockRequestResponse for %s ( worker %lu error %s)"
                                     , str_block_id(block_id).c_str()
                                     , worker_id
                                     , error?"yes":"no" ));
    
    if( error )
    {
      BlockRequest* block_request = block_requests_.findInMap( block_id );
      if( block_request )
        block_request->GotErrorMessage( worker_id );
      return;
    }
    
    // If no error, just remove the request...
    BlockRequest* block_request = block_requests_.extractFromMap( block_id );
    delete block_request;
    
  }
  
  au::SharedPointer<gpb::Collection> WorkerBlockManager::GetCollectionForBlockRequests(const Visualization& visualization) {
    return GetCollectionForMap("block_requests", block_requests_, visualization);
  }
  
  void WorkerBlockManager::RequestBlock( size_t block_id ) {
    
    BlockRequest* block_request = block_requests_.findInMap(block_id);
    if (block_request)
    {
      AU_M(logs.worker_block_manager, ("Requested block %s.... found in Block Manager!", str_block_id(block_id).c_str() ));
      return; // already requested
    }

    if( block_requests_.findInMap(block_id) )
    {
      AU_M(logs.worker_block_manager, ("Requested block %s.... previously requestes and still waiting!", str_block_id(block_id).c_str() ));
      return;
    }
    
    // New block request for this block
    AU_M(logs.worker_block_manager, ("Requested block %s", str_block_id(block_id).c_str() ));
    block_request = new BlockRequest( samson_worker_, block_id );
    block_requests_.insertInMap(block_id, block_request);
    
  }
  
  void WorkerBlockManager::RequestBlocks( const std::set<size_t>& pending_block_ids ) {
    std::set<size_t>::const_iterator it;
    for (it = pending_block_ids.begin(); it != pending_block_ids.end(); ++it) {
      size_t block_id = *it;      // Identifier of the block
      RequestBlock(block_id);
    }
  }
  
  void WorkerBlockManager::Reset() {
    AU_M(logs.worker_block_manager, ("Reset"));
    // Remove all internal elements
    block_requests_.clearMap();
  }
  
  // Received a message from a delilah
  void WorkerBlockManager::ReceivedPushBlock( size_t delilah_id
                                             , size_t push_id
                                             , engine::BufferPointer buffer
                                             , const std::vector<std::string>& queues) {
    
    AU_M(logs.worker_block_manager, ("Received a push block (Delilah %s PushId %lu Buffer %s Queues %s)"
                                     , au::code64_str(delilah_id).c_str()
                                     , push_id
                                     , buffer->str().c_str()
                                     , au::str( queues ).c_str() ));
    
    if (buffer == NULL) {
      AU_W( logs.worker_block_manager, ("Push message without a buffer. This is an error..."));
      return;
    }
    
    // Check valid header size
    size_t block_size = buffer->size();
    if (buffer->size() < sizeof(KVHeader)) {
      AU_W(logs.worker_block_manager,("Push message with a non-valid buffer.Ignoring..."));
      return;
    }
    
    KVHeader *header = reinterpret_cast<KVHeader *>(buffer->data());
    
    if (!header->Check() || !header->range.isValid()) {
      AU_W(logs.worker_block_manager,("Push message with a non-valid buffer.Ignoring..."));
      return;
    }
    
    if (header->IsTxt()) {
      // Random hash-group based on all my ranges
      std::vector<KVRange> ranges = samson_worker_->worker_controller()->GetMyKVRanges();
      std::vector<size_t> all_hgs;
      for (size_t i = 0 ; i < ranges.size() ; i++ ) {
        for ( int j = ranges[i].hg_begin_; j < ranges[i].hg_end_; j++) {
          all_hgs.push_back(j);
        }
      }
      int hg =  all_hgs[ rand()%all_hgs.size()];
      header->range.set(hg, hg + 1);
    } else if (header->IsModule()) {
      header->range.set(0, KVFILE_NUM_HASHGROUPS);      // Make sure it is full range
    } else {
      AU_W(logs.worker_block_manager,("Push message with a buffer that is not data or a module.Ignoring..."));
      return;
    }
    
    // Create a new block in this worker ( and start distribution process )
    size_t block_id = samson_worker_->worker_block_manager()->CreateBlock(buffer);
    
    if (block_id == (size_t) -1) {
      AU_W(logs.worker_block_manager,("Error creating block in a push operation ( block_id -1 )"));
      return;
    }
    
    // Get commit command for this push operation
    CommitCommand commit_command;
    for (size_t i = 0; i < queues.size(); i++) {
      commit_command.AddBlock(queues[i], block_id, block_size, header->GetKVFormat(), header->range, header->info);
    }
    
    // Commit to data model
    std::string command = commit_command.GetCommitCommand();
    au::ErrorManager error;
    std::string caller = au::str("PushOperation from delilah %s ( push id %lu )"
                                 , au::code64_str(delilah_id).c_str()
                                 , push_id);
    samson_worker_->data_model()->Commit(caller, command, error);
    
    if( error.IsActivated() )
    {
      AU_W(logs.worker_block_manager,("Error comitting a push operation to data model: %s" , error.GetMessage().c_str() ));
      return;
    }
    
    // Send a message to delilah to inform we have received correctly
    PacketPointer packet(new Packet(Message::PushBlockResponse));
    packet->to = NodeIdentifier(DelilahNode, delilah_id);
    packet->message->set_push_id(push_id);
    samson_worker_->network()->Send(packet);
    return;
  }
  
}
