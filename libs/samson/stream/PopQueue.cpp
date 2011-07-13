


#include "engine/Engine.h"
#include "engine/Notification.h"

#include "samson/common/NotificationMessages.h"     // notification_samson_worker_send_packet

#include "samson/network/Packet.h"                  // network::Packet

#include "samson/module/ModulesManager.h"           // samson::ModulesManager


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
        
        void PopQueue::run( Queue * q )
        {
            
            if( !q )
            {
                error.set( au::Format::string("Unknown queue %s" , pq->target().queue().c_str() ));
                check();
                return;
            }
            
            std::string operation_name = pq->parserout();
            samson::Operation *op = ModulesManager::shared()->getOperation( operation_name );
            
            if( !op ) 
            {
                error.set( au::Format::string("Unknown operation %s" , operation_name.c_str() ));
                check();
                return;
            }
            
            if( op->getType() != samson::Operation::parserOut )
            {
                error.set( au::Format::string("Operation %s is not a parserOut. Nice try! ;) " , operation_name.c_str() ));
                check();
                return;
            }
            
            // Pending check format
            // KVFormat inputFormat =  op->getInputFormat(0);
            
            // Schedule all the operation
            q->scheduleTasksForPopQueue( this );
            
            //num_stream_tasks = 10;
            //num_finished_stream_tasks = 0;
            
            check();
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
            output << getQueue() << ":" << getChannel() << " --> Pending " << running_tasks.size() << " tasks";
            return output.str();
        }
        
        void PopQueue::sendFinalMessage()
        {
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

        void PopQueue::sendMessage( engine::Buffer *buffer )
        {
            if( finished )
            {
                LM_W(("Sending data to a finish pop-queue. Ignoring and deleting packet..."));
                engine::MemoryManager::shared()->destroyBuffer( buffer );
                return;
            }
            
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
        
        
    }
}