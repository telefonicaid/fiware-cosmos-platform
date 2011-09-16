


#include "engine/Engine.h"
#include "engine/Notification.h"

#include "samson/common/NotificationMessages.h"     // notification_samson_worker_send_packet

#include "samson/network/Packet.h"                  // network::Packet

#include "samson/module/ModulesManager.h"           // samson::ModulesManager


#include "Block.h"                                  // samson::stream::Block
#include "BlockList.h"                              // samson::stream::BlockList

#include "Queue.h"                                  // samson::stream::Queue

#include "PopQueue.h"                               // Own interface

namespace samson {
    namespace stream {
        
        
        PopQueue::PopQueue(const network::PopQueue& _pq , size_t _delilahId, int _fromId  )
        {
            pq = new network::PopQueue();
            pq->CopyFrom( _pq );
            delilahId = _delilahId;
            fromId = _fromId;

            // Init the finish flag
            finished =  false;
            
        }
        
        PopQueue::~PopQueue()
        {
            delete pq;
        }
        
        void PopQueue::addTask( size_t id )
        {
            running_tasks.insert( id );
        }
        
        void PopQueue::check()
        {
            // If the pop queue operation is already finished, do nothing...
            if( finished )
                return;
            
            // If we have an error here, just answer to delilah
            if( error.isActivated() )
            {
                finished = true;
                sendFinalMessage();
            }
            else if( running_tasks.size() == 0 )
            {
                finished = true;
                sendFinalMessage();
            }
        }
        
        std::string PopQueue::getStatus()
        {
            std::ostringstream output;
            output << pq->queue() << " --> Pending " << running_tasks.size() << " tasks";
            return output.str();
        }
        
        void PopQueue::sendFinalMessage()
        {
            //LM_M(("Sending final message..."));
            
            // Send the packet using notification mecanism
            samson::Packet *packet = new Packet( Message::PopQueueResponse );
            
            // Set delialh id
            packet->message->set_delilah_id( delilahId );

            
            // Set the finish flag to true
            network::PopQueueResponse* pqr =  packet->message->mutable_pop_queue_response();
            pqr->set_finish(true);

            if( error.isActivated() )
            {
                network::Error *_error = pqr->mutable_error();
                _error->set_message(error.getMessage());
            }
            
            engine::Notification *notification = new engine::Notification( notification_samson_worker_send_packet , packet );
            notification->environment.set("toId", fromId );
            
            // Send a notification
            engine::Engine::shared()->notify( notification );
            
        }


        
        void PopQueue::notifyFinishTask( size_t task_id , au::ErrorManager *_error )
        {
            
            running_tasks.erase( task_id );
            error.set( _error );
                        
            check();
        }

        
#pragma marg PopQueueTask
        

        PopQueueTask::PopQueueTask( size_t _id , PopQueue *pq , KVRange _range ) : engine::ProcessItem( PI_PRIORITY_NORMAL_OPERATION )
        {
            id = _id;
            pop_queue_id =  pq->id;
            
            range = _range;
            
            delilahId = pq->delilahId;
            fromId = pq->fromId;
            
            list = new BlockList("PopQueueTask");
            list_lock = new BlockList("PopQueueTask_lock");
            
            ready_flag = false;
            
            // Set the id
            environment.setSizeT("id", id);
            environment.set("type","pop_queue_task");
            
        }
        
        PopQueueTask::~PopQueueTask()
        {
            delete list;
            delete list_lock;
        }

        void PopQueueTask::addBlock( Block*b )
        {
            list->add( b );
        }
        
        
        bool PopQueueTask::ready()
        {
            if( ready_flag )
                return true;
            
            ready_flag = (list->isContentOnMemory() ) ;
            
            if( ready_flag )
            {
                list_lock->copyFrom( list );
            }
            
            return ready_flag;
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
            
            LM_M(("PopQueueTask: running over %d blocks" , (int) list_lock->blocks.size() ));
            
            au::list<Block>::iterator b;
            for ( b = list_lock->blocks.begin() ; b != list_lock->blocks.end() ; b++)
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
