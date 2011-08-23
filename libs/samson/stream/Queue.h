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

#include "au/list.h"      // au::list
#include "au/Info.h"      // au::Info

#include "au/Cronometer.h"                  // au::cronometer

#include "samson/common/coding.h"           // FullKVInfo

#include "samson/common/samson.pb.h"        // network::

#include "engine/Object.h"                  // engine::Object

#include "samson/stream/BlockList.h"        // samson::Stream::BlockList


namespace samson {
    
    class NetworkInterface;
    class Info;
    
    namespace stream
    {
        class Block;
        class QueuesManager;
        class PopQueue;
        
        class Queue : engine::Object
        {
            friend class QueuesManager; 
            
            QueuesManager * qm;                 // Pointer to the queue manager
            
            std::string name;                   // Name of the queue

            au::Cronometer cronometer;          // Time since the last command execution
                        
        public:

            BlockList *list;                     // List of blocks contained in this queue
            
            
            Queue( std::string _name , QueuesManager * _qm );
            ~Queue();
                       
            std::string getStatus();
            
            // Notifications    
            //void notify( engine::Notification* notification );
            
            // Get information for monitorization
            void getInfo( std::ostringstream& output);
            
            
        };
        
        
    }
}

#endif
