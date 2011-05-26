#ifndef _H_STREAM_QUEUE
#define _H_STREAM_QUEUE

/* ****************************************************************************
 *
 * FILE                      Queue.h
 *
 * AUTHOR                    Andreu Urruela Planas
 *
 * All the information related with a queue ( data and state )
 *
 */

#include <ostream>      // std::ostream
#include <string>       // std::string
#include <list>          // std::list

#include "samson/common/samson.pb.h"    // network::

namespace samson {
    namespace stream
    {
        class Block;
        
        class Queue 
        {

            friend class QueuesManager;
            
            std::string name;               // Name of the queue
            std::list< Block* > blocks;      // Blocks currently in the input queue
            
            // Information about how to process this queue ( from controller )
            network::StreamQueue *streamQueue;
            
        public:
            
            Queue( std::string _name )
            {
                name = _name;
                streamQueue = NULL; // By default it is not assigned
            }
            
            ~Queue()
            {
                if( streamQueue )
                    delete streamQueue;
            }
            
            void setStreamQueue( network::StreamQueue& _streamQueue )
            {
                // Only if not set before
                if( ! streamQueue )
                {
                    streamQueue = new network::StreamQueue();
                    streamQueue->CopyFrom(_streamQueue);
                }
            }
            
            void add( Block *block );

            
            std::string getStatus();
            
        };
        
        
    }
}

#endif