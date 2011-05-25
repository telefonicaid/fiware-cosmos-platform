

#ifndef _H_STREAM_QUEUE_MANAGER
#define _H_STREAM_QUEUE_MANAGER

/* ****************************************************************************
 *
 * FILE                      QueuesManager.h
 *
 * AUTHOR                    Andreu Urruela Planas
 *
 * All the queues contained in the system
 *
 */


#include "au/map.h"     // au::map
#include <string>

namespace samson {
    namespace stream
    {
        
        class Queue;
        class Block;
        
        class QueuesManager 
        {
            au::map< std::string , Queue > queues;  // Map with the current queues
     
        public:
            
            QueuesManager();
            std::string getStatus();

            // Add a block to a particular queue ( ususally from the network interface )
            void addBlock( std::string queue , Block *b);
            
        };
    }
}

#endif
