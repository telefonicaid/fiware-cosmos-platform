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
            
            delilah_id   = pq->delilah_id;
            delilah_component_id   = pq->delilah_component_id;
            
            environment.set("system.pop_queue_task","yes");
            
            // Additional information
            environment.setSizeT("system.pop_queue_id", pop_queue_id );
            
        }
        
        
        
        void PopQueueTask::sendMessage( engine::Buffer *buffer )
        {
            
            //LM_M(("PopQueueTask: Sending message with size %lu" , buffer->getSize() ));
            
            // Send the packet using notification mecanism
            samson::Packet *packet = new Packet( Message::PopQueueResponse );
            
            // Set the buffer to be sent to this delilah
            packet->setBuffer( buffer );
            
            // Set delialh id
            packet->message->set_delilah_component_id( delilah_component_id );
            
            // Set the finish flag to true
            network::PopQueueResponse* pqr =  packet->message->mutable_pop_queue_response();
            pqr->set_finish(false); // Not a final message
            

            // Direction of the packet
            packet->to.node_type = DelilahNode;
            packet->to.id = delilah_id;
            
            // Send a notification
            engine::Notification *notification = new engine::Notification( notification_samson_worker_send_packet , packet );
            engine::Engine::shared()->notify( notification );            
            
            // Release the packet ( now it is retained by, at least, the notification )
            packet->release();

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
                    
                    // Buffer is suppoused to be in memory
                    engine::Buffer* buffer = block->buffer_container.getBuffer();

                    // Send the buffer ( will be retained by the packet )
                    sendMessage( buffer );
                }
                else
                    LM_W(("PopQueueTask: Found a block that is not in memory. This shoudl be an error...skipping"));
                
            }
            
        }
        
#pragma marg StreamOutQueueTask
            
        StreamOutQueueTask::StreamOutQueueTask( size_t _id , size_t _fromId, std::string _queue )
            : SystemQueueTask( _id , au::str("StreamOut queue ( %s )" , _queue.c_str() ) )
        {
            fromId  = _fromId;
            queue   = _queue;
        }
        
        
        
        void StreamOutQueueTask::sendMessage( engine::Buffer *buffer )
        {
            
            // Send the packet using notification mecanism
            samson::Packet *packet = new Packet( Message::StreamOutQueue );
            
            // Send to the rigth delilah
            packet->to = NodeIdentifier( DelilahNode , fromId );
            
            // Set the buffer to be sent to this delilah
            packet->setBuffer( buffer );
            
            // Set no delialh id ( this is always a top level message )
            packet->message->set_delilah_component_id( 0 );
            
            // Set the finish flag to true
            network::StreamOutQueue* network_stream_output_queue =  packet->message->mutable_stream_output_queue();
            network_stream_output_queue->set_queue( queue );
            
            engine::Notification *notification = new engine::Notification( notification_samson_worker_send_packet , packet );
            
            // Send a notification
            engine::Engine::shared()->notify( notification );            
            
            // Release the packet ( now it is retained by, at least, the notification )
            packet->release();

        }
        
        void StreamOutQueueTask::run()
        {
            
            BlockList *list = getBlockList("input_0");
            au::list<Block>::iterator b;
            for ( b = list->blocks.begin() ; b != list->blocks.end() ; b++)
            {
                Block *block = *b;
                
                if( block->isContentOnMemory() )
                {
                    sendMessage( block->buffer_container.getBuffer() );
                }
                else
                    LM_W(("PopQueueTask: Found a block that is not in memory. This shoudl be an error...skipping"));
                
            }
            
        }
        
        
    }
}
