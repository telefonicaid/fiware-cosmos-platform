

#include "QueuesManager.h"  // Own interface
#include <string>           // std::string
#include <sstream>          // std::stringstream
#include "Queue.h"          // samson::stream::Queue
#include "Block.h"          // samson::stream::Block

#include "engine/Engine.h"              // engine::Engine
#include "engine/MemoryManager.h"
#include "engine/Notification.h"

#include "samson/stream/PopQueue.h" // stream::PopQueue

#include "samson/worker/SamsonWorker.h"

#include "samson/stream/QueueTask.h"


#include "samson/stream/Queue.h"

namespace samson {
    namespace stream{
    
        QueuesManager::QueuesManager(::samson::SamsonWorker* _worker) : queueTaskManager( this )
        {
            worker = _worker;
            
            // add a generic periodic notification to check tasks for stream queues
            engine::Notification *notification = new engine::Notification( notification_review_task_for_queue );
            engine::Engine::shared()->notify( notification , 1 );
            
        }
        
        std::string QueuesManager::getStatus()
        {
            
            std::ostringstream output;
            
            output << "Queues:\n";
            
            au::map< std::string , Queue >::iterator q;
            for ( q = queues.begin() ; q != queues.end() ; q++)
                output <<  au::Format::indent( q->second->getStatus()  ) << "\n";

            // Queue task Manager status
            output << queueTaskManager.getStatus();

            // Queue pop quuee
            output << popQueueManager.getStatus();
            
            return output.str();
            
        }


        void QueuesManager::addBlock( std::string queue_name , int channel ,  Block *b )
        {
            // Get or create the queue
            Queue *queue = getQueue( queue_name );
            
            // Add the block to the queue
            queue->add( channel , b );
            
            // First rudimentary system to thrigger queue-processing
            queue->scheduleNewTasksIfNecessary();
            
        }
     
        void QueuesManager::setInfo( network::StreamQueue &stream_queue )
        {
            // Get or create the queue
            Queue *queue = getQueue( stream_queue.name() );
            queue->setStreamQueue( stream_queue );
            
        }
        
        Queue* QueuesManager::getQueue( std::string queue_name )
        {
            Queue *queue = queues.findInMap( queue_name );
            if (! queue )
            {
                queue = new Queue( queue_name , this );
                queues.insertInMap( queue->name, queue );
            }
            
            return queue;
            
        }

        void QueuesManager::notifyFinishTask( QueueTask *task )
        {
            
            std::string queue  = task->environment.get("queue","--");
            size_t task_id = task->id;
            
            //LM_M(("Operation %lu finished for queue %s", task_id, queue.c_str() ));
            
            if( queue != "--" )
                getQueue( queue )->notifyFinishTask( task_id );
            
            
            // Notify to the popQueue operations
            size_t pop_queue_id = task->environment.getSizeT("pop_queue_id", 0);
            
            //LM_M(("notify finish task for pop_queue_id %lu" , pop_queue_id));
            
            if( pop_queue_id != 0 )
                popQueueManager.notifyFinishTask( pop_queue_id , task_id , &task->error );
            
        }

        void QueuesManager::addPopQueue(const network::PopQueue& pq , size_t delilahId, int fromId )
        {
            std::string queue_name = pq.target().queue();
            Queue* q = queues.findInMap( queue_name );
            
            PopQueue *popQueue = new PopQueue( pq , delilahId, fromId );
            popQueueManager.add( popQueue );
            
            // Run the pop queue operation with the current queue information
            popQueue->run( q );
        }
        

        
    }

}