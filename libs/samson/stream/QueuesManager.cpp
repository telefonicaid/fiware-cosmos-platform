

#include "QueuesManager.h"  // Own interface
#include <string>           // std::string
#include <sstream>          // std::stringstream
#include "Queue.h"          // samson::stream::Queue
#include "Block.h"          // samson::stream::Block

#include "engine/Engine.h"              // engine::Engine
#include "engine/MemoryManager.h"
#include "samson/worker/SamsonWorker.h"

#include "samson/stream/Queue.h"

namespace samson {
    namespace stream{
    
        QueuesManager::QueuesManager(::samson::SamsonWorker* _worker)
        {
            worker = _worker;
            
            // add a generic periodic notification to check tasks for stream queues
            engine::Notification *notification = new engine::Notification( notification_review_task_for_queue );
            engine::Engine::add( notification , 3 );
            
        }
        
        std::string QueuesManager::getStatus()
        {
            
            std::ostringstream output;
            
            output << "\tQueues:\n";
            au::map< std::string , Queue >::iterator q;
            for ( q = queues.begin() ; q != queues.end() ; q++)
                output << "\t\t" << q->second->getStatus() <<"\n";
            output << queueTaskManager.getStatus();
           
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


        
    }

}