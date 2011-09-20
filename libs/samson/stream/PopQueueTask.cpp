
#include "engine/Engine.h"
#include "engine/Notification.h"

#include "samson/common/NotificationMessages.h"     // notification_samson_worker_send_packet
#include "samson/common/MemoryTags.h"               // MemoryBlocks

#include "samson/network/Packet.h"                  // network::Packet

#include "samson/module/ModulesManager.h"           // samson::ModulesManager


#include "Block.h"                                  // samson::stream::Block
#include "BlockList.h"                              // samson::stream::BlockList
#include "PopQueue.h"                               // samson::stream::PopQueue
#include "Queue.h"                                  // samson::stream::Queue

#include "PopQueueTask.h"                               // Own interface

namespace samson {
    namespace stream {
        
        
        
#pragma marg PopQueueTask
        
        
        PopQueueTask::PopQueueTask( size_t _id , PopQueue *pq , KVRange _range ) 
            : SystemQueueTask( _id , au::str("Pop queue ( %s )" , pq->pq->queue().c_str() ) )
        {
            pop_queue_id =  pq->id;
            
            range = _range;
            
            delilahId = pq->delilahId;
            fromId = pq->fromId;
            
            environment.set("system.pop_queue_task","yes");
            
            // Additional information
            environment.setSizeT("system.pop_queue_id", pop_queue_id );
            
        }
        
        
        
        void PopQueueTask::sendMessage( engine::Buffer *buffer )
        {
            
            LM_M(("PopQueueTask: Sending message with size %lu" , buffer->getSize() ));
            
            // Send the packet using notification mecanism
            samson::Packet *packet = new Packet( Message::PopQueueResponse );
            
            // Set the buffer to be sent to this delilah
            packet->buffer = buffer;
            
            // Set delialh id
            packet->message->set_delilah_id( delilahId );
            
            // Set the finish flag to true
            network::PopQueueResponse* pqr =  packet->message->mutable_pop_queue_response();
            pqr->set_finish(false); // Not a final message
            
            
            engine::Notification *notification = new engine::Notification( notification_samson_worker_send_packet , packet );
            notification->environment.set("toId", fromId );
            
            // Send a notification
            engine::Engine::shared()->notify( notification );            
        }
        
        void PopQueueTask::run()
        {
            
            BlockList *list = getBlockList("input_0");
            au::list<Block>::iterator b;
            for ( b = list->blocks.begin() ; b != list->blocks.end() ; b++)
            {
                Block *block = *b;
                
                if( block->isContentOnMemory() )
                {
                    
                    size_t  size = block->buffer->getSize();
                    
                    engine::Buffer *buffer = engine::MemoryManager::shared()->newBuffer("PopQueueTask_run", size, 0 );
                    
                    memcpy( buffer->getData(), block->buffer->getData(), size );
                    buffer->setSize(size);
                    
                    sendMessage( buffer );
                }
                else
                    LM_W(("PopQueueTask: Found a block that is not in memory. This shoudl be an error...skipping"));
                
            }
            
        }
        
    }
}
