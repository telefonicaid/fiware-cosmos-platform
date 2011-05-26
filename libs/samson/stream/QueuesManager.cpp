

#include "QueuesManager.h"  // Own interface
#include <string>           // std::string
#include <sstream>          // std::stringstream
#include "Queue.h"          // samson::stream::Queue
#include "Block.h"          // samson::stream::Block

#include "engine/MemoryManager.h"

namespace samson {
    namespace stream{
    
        QueuesManager::QueuesManager()
        {
            
        }
        
        std::string QueuesManager::getStatus()
        {
            
            std::ostringstream output;
            
            au::map< std::string , Queue >::iterator q;
            for ( q = queues.begin() ; q != queues.end() ; q++)
                output << "\n" << q->second->getStatus();
            
            return output.str();
            
        }


        void QueuesManager::addBlock( std::string queue_name , Block *b)
        {
            // Get or create the queue
            Queue *queue = getQueue( queue_name );
            
            // Add the block to the queue
            queue->add( b );
            
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
                queue = new Queue( queue_name );
                queues.insertInMap( queue->name, queue );
            }
            
            return queue;
            
        }


        
    }

}