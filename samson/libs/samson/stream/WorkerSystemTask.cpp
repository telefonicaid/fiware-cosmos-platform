
#include "engine/Engine.h"
#include "engine/Notification.h"

#include "samson/common/NotificationMessages.h"
// MemoryBlocks
#include "samson/common/MessagesOperations.h"

#include "samson/stream/BlockManager.h"
#include "samson/worker/SamsonWorker.h"

#include "samson/network/Packet.h"                  // network::Packet

#include "samson/module/ModulesManager.h"           // samson::ModulesManager


#include "Block.h"                                  // samson::stream::Block
#include "BlockList.h"                              // samson::stream::BlockList


#include "WorkerSystemTask.h"                       // Own interface

namespace samson {
  namespace stream {
    
    
    
    // ------------------------------------------------------------------------
    //
    // BlockRequestTask
    //
    // ------------------------------------------------------------------------
    
    
    BlockDistributionTask::BlockDistributionTask(SamsonWorker *samson_worker
                                                 , size_t id
                                                 , size_t block_id
                                                 , const std::vector<size_t>& worker_ids)
    : WorkerTaskBase(samson_worker,id, au::str("BlockDistribution %lu to workers %s", block_id , au::str(worker_ids).c_str() ))
    {
      block_id_ = block_id;
      worker_ids_ = worker_ids;
      
      // Selected block
      block_ = stream::BlockManager::shared()->GetBlock(block_id);
      
      if (block_ == NULL) {
        LM_W(("DistributeBlockTask for unknown block %lu", block_id));
      }
      AddInput(0, block_, block_->getKVRange(), block_->getKVInfo());
      
      std::string message = au::str("<< Block distribution %lu to workers %s >>"
                                    , block_id
                                    , au::str(worker_ids_).c_str());
      environment().Set("worker_command_id", message);
    }
    
    // Virtual method from engine::ProcessItem
    void BlockDistributionTask::run() {
      if (block_ == NULL) {
        return;     // Nothing to distribute
      }
      // Block is suppouse to be on memory since this is a task
      if (!block_->is_content_in_memory()) {
        LM_X(1, ("Internal error"));
      }
      
      // Send a packet to all selected workers
      for (size_t i = 0; i < worker_ids_.size(); i++) {
        PacketPointer packet(new Packet(Message::BlockDistribution));
        packet->set_buffer(block_->buffer());
        packet->message->set_block_id(block_id_);
        packet->to = NodeIdentifier(WorkerNode, worker_ids_[i]);
        
        // Sending a engine notification to really sent this packet
        engine::Notification *notification = new engine::Notification(notification_send_packet);
        notification->dictionary().Set<Packet>("packet",  packet );
        engine::Engine::shared()->notify(notification);
      }
    }
    
    // ------------------------------------------------------------------------
    //
    // PopBlockRequestTask
    //
    // ------------------------------------------------------------------------
    
    PopBlockRequestTask::PopBlockRequestTask(SamsonWorker *samson_worker
                                             , size_t id
                                             , size_t block_id
                                             , const gpb::KVRanges& ranges
                                             , size_t delilah_id
                                             , size_t delilah_component_id
                                             , size_t pop_id)
    : WorkerTaskBase(samson_worker , id, au::str("BlockPopRequest %lu [ delilah %lu ]", block_id, delilah_id) )
    {
      block_id_ = block_id;
      ranges_.CopyFrom(ranges);
      
      delilah_id_ = delilah_id;
      delilah_component_id_ = delilah_component_id;
      pop_id_ = pop_id;
      
      // Selected block
      block_ = stream::BlockManager::shared()->GetBlock(block_id);
      
      // add this block as input to make sure it is in memory when task is executed
      AddInput(0, block_, block_->getKVRange(), block_->getKVInfo());
    }
    
    // Virtual method from engine::ProcessItem
    void PopBlockRequestTask::run() {
      if (!block_->is_content_in_memory()) {
        LM_X(1, ("Internal error"));      // Get kv file for this block
      }
      au::SharedPointer<KVFile> kv_file = block_->getKVFile(error_);
      if (error_.IsActivated()) {
        LM_W(("Not possible to get KVFile for block in a PopBlockRequestTask"));
        return;
      }
      
      // Transform to c++ ranges class ( not gpb )
      KVRanges ranges(ranges_);      // Implicit conversion
      
      // Full range ( TBC )
      if (ranges.IsFullRange()) {
        sent_response(block_->buffer());
        return;
      }
      
      // txt-txt buffers
      if (kv_file->header().isTxt()) {
        // If ranges includes
        if (ranges.IsOverlapped(block_->getHeader().range)) {
          sent_response(block_->buffer());
        } else {
          sent_response(engine::BufferPointer(NULL));
        } return;
      }
      
      
      // Compute total size
      KVInfo info;
      for (int i = 0; i < KVFILE_NUM_HASHGROUPS; i++) {
        if (ranges.Contains(i)) {
          info.append(kv_file->info[i]);
        }
      }
      
      // Build selected packet filtering with ranges_
      size_t buffer_size = sizeof(KVHeader) + info.size;
      engine::BufferPointer buffer(engine::Buffer::Create("test", "example", buffer_size));
      buffer->set_size(buffer_size);
      
      // Copy header
      KVHeader header = block_->getHeader();
      header.info = info;      // Replaces with new info
      memcpy(buffer->data(), &header, sizeof(KVHeader));
      
      // Copy content for each hash-group
      
      char *source_data = kv_file->data;
      char *target_data = buffer->data() + sizeof(KVHeader);
      
      for (int i = 0; i < KVFILE_NUM_HASHGROUPS; i++) {
        size_t size = kv_file->info[i].size;
        
        if (ranges.Contains(i)) {
          memcpy(target_data, source_data, size);
          target_data += size;      // Move pointer to the next
        }
        source_data += size;      // Move pointer to the next
      }
      
      // Sent response with computed buffer
      sent_response( buffer );
      
    }
    
    void PopBlockRequestTask::sent_response(engine::BufferPointer buffer) {
      // Send a packet to delilah with generated content
      LM_W(("****** Sending PopBlockRequestResponse"));
      PacketPointer packet(new Packet(Message::PopBlockRequestResponse));
      
      packet->to = NodeIdentifier(DelilahNode, delilah_id_);
      packet->message->set_block_id(block_id_);
      packet->message->set_delilah_component_id(delilah_component_id_);
      packet->message->set_pop_id(pop_id_);
      packet->set_buffer(buffer);
      
      // Sending a engine notification to really sent this packet
      engine::Notification *notification = new engine::Notification(notification_send_packet);
      notification->dictionary().Set<Packet>("packet",  packet);
      engine::Engine::shared()->notify(notification);
    }
    
    void DefragTask::AddOutputBuffer( engine::BufferPointer buffer) {
      
      // Information for generated block
      KVHeader *header = (KVHeader *)buffer->data();
      
      // Create a block ( and distribute it )
      size_t block_id = samson_worker_->worker_block_manager()->CreateBlock(buffer);
      BlockPointer block = BlockManager::shared()->GetBlock(block_id);
      
      // Add output to this operation
      AddOutput(0, block, header->range, header->info);
      
    }
    
    void DefragTask::run()
    {
      // Review input blocks
      BlockList *list = block_list_container_.getBlockList("input_0");
      list->ReviewBlockReferences(error_);

      if( error_.IsActivated() )
      {
        AU_LM_W((">>>> Error in defrag operation: %s" , error_.GetMessage().c_str() ));
        return;
      }
      
      // Get vector with all KVFiles at the input
      // ------------------------------------------------------------------------------
      std::vector< au::SharedPointer<KVFile> > kv_files;
      au::list< BlockRef >::iterator bi;
      for (bi = list->blocks.begin(); bi != list->blocks.end(); bi++) {
        BlockRef *block_ref = *bi;
        BlockPointer block = block_ref->block();
        engine::BufferPointer buffer = block->buffer();
        
        if( buffer == NULL )
        {
          error_.set( au::str("Block %lu is apparently not in memory" , block_ref->block_id() ));
          return;
        }
        
        // Check header for valid block
        KVHeader *header = (KVHeader *)buffer->data();
        if (!header->check()) {
          error_.set("Not valid header in block reference");
          return;
        }
        
        // Analyse all key-values and hashgroups
        au::SharedPointer<KVFile> file = block_ref->file();
        
        if (file == NULL) {
          error_.set( au::str("Error getting KVFile for block %lu" , block_ref->block_id() ));
          return;
        }
        
        kv_files.push_back( file );
      }

      if( kv_files.size() == 0 )
      {
        error_.set("No data provided for defrag operation");
        return;
      }
      

      // Generate output buffers putting toguether all data for each hash-group
      // ------------------------------------------------------------------------------

      KVHeader header = kv_files[0]->header();
      
      // Process all kv_files
      int hg_begin = 0;
      while (hg_begin < KVFILE_NUM_HASHGROUPS) {
        
        // Search for the end
        int hg_end = hg_begin;
        size_t accumulated_size = 0;
        size_t accumulated_kvs = 0;
        while( true )
        {
          if( hg_end >= KVFILE_NUM_HASHGROUPS)
            break;
          
          size_t tmp_size = 0;
          size_t tmp_kvs = 0;
          for ( size_t i = 0 ; i < kv_files.size() ; i++ )
          {
            tmp_size += kv_files[i]->info[hg_end].size;
            tmp_kvs += kv_files[i]->info[hg_end].kvs;
          }
          
          
          if( accumulated_size > 0 )
            if( ( accumulated_size + tmp_size) > 100000000 )
              break;
          
          accumulated_size += tmp_size;
          accumulated_kvs += tmp_kvs;
          
          hg_end++;
        }
        
        // Create output buffer ( hg_begin hg_end )
        size_t buffer_size = accumulated_size + sizeof(KVHeader);
        engine::BufferPointer buffer = engine::Buffer::Create("defrag", "normal", buffer_size);
        buffer->set_size( buffer_size );
        
        // Copy header and modify info
        KVHeader* buffer_header = (KVHeader*)buffer->data();
        char* buffer_data = buffer->data();
        
        memcpy(buffer_header, &header, sizeof(KVHeader));
        buffer_header->info.set(accumulated_size, accumulated_kvs);
        
        // Copy data
        size_t offset = sizeof(KVHeader);
        for ( int hg = hg_begin ; hg < hg_end ; hg++ )
          for ( size_t i = 0 ; i < kv_files.size() ; i++ )
          {
            size_t size = kv_files[i]->info[hg].size;
            if ( size > 0 )
            {
              char* data =kv_files[i]->data_for_hg(hg);
              memcpy(buffer_data+offset, data, size);
              offset += size;
            }
          }
        
        if( offset != buffer_size)
        {
          error_.set("Internal error in defrag operation");
          return;
        }
        
        // add generated buffer to the output
        AddOutputBuffer(buffer);
        
        // Continue creating blocks
        hg_begin = hg_end;
        
      }
      
    }
    
    std::string DefragTask::commit_command() {
      std::vector<std::string> inputs;
      std::vector<std::string> outputs;
      inputs.push_back(queue_name_);
      outputs.push_back(queue_name_);
      return generate_commit_command( inputs , outputs );
    }

    
  }
}

