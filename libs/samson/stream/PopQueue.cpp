


#include "engine/Engine.h"
#include "engine/Notification.h"

#include "samson/common/NotificationMessages.h"     // notification_samson_worker_send_packet
#include "samson/common/MemoryTags.h"               // MemoryBlocks

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
        
    }

}