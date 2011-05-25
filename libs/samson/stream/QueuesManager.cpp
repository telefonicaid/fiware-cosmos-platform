

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
            Queue *queue = queues.findInMap( queue_name );
            if (! queue )
            {
                queue = new Queue( queue_name );
                queues.insertInMap( queue->name, queue );
            }
            
            // Add the block to the queue
            queue->add( b );
            
        }
        
        
    }

}