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

namespace ss {
    namespace stream
    {
        class Block;
        
        class Queue 
        {

            friend class QueuesManager;
            
            std::string name;               // Name of the queue
            std::list< Block* > blocks;      // Blocks currently in the input queue
            
        public:
            
            Queue( std::string _name )
            {
                name = _name;
            }
            
            void add( Block *block )
            {
                // Insert in the back of the list
                blocks.push_back( block );
            }

            
            std::string getStatus();
            
        };
        
        
    }
}

#endif