


#ifndef _H_SAMSON_STREAM_QUEUE_CONNECTIONS
#define _H_SAMSON_STREAM_QUEUE_CONNECTIONS

#include <string>                       // std::string

#include "au/list.h"                    // au::list
#include "au/Environment.h"             // au::Environment
#include "au/Rate.h"                    // au::Rate

#include "samson/common/Rate.h"         // samson::Rate
#include "samson/common/coding.h"       // KVFullInfo
#include "samson/common/samson.pb.h"    // samson::network::...
#include "samson/common/MessagesOperations.h"

#include "BlockInfo.h"                              // struct BlockInfo

namespace samson {
    
    class QueueConnections
    {
    public:
        
        std::string queue;                    // Just a copy of the queue name we are extracting data
        std::set<std::string> target_queues;  // List of queue names to push data 
                
        void add_connection( std::string target_queue )
        {
            target_queues.insert( target_queue );
        }
        
        void remove_connection( std::string target_queue )
        {
            target_queues.erase( target_queue );
        }
        
        
        bool hasConnections()
        {
            return ( target_queues.size() != 0 );
        }
        
        void fill( samson::network::CollectionRecord* record , Visualization* visualitzation )
        {
            add( record , "queue" , queue , "left,different" );


            add( record , "connected to.." , str_target_queues() , "left,different" );
            
        }
        
        std::string str_target_queues()
        {
            std::ostringstream output;
            std::set<std::string>::iterator it_target_queues;
            for ( it_target_queues = target_queues.begin() ; it_target_queues != target_queues.end() ;  )
            {
                output << *it_target_queues;
                it_target_queues ++;
                
                if ( it_target_queues != target_queues.end() )
                    output << " ";
            }
            
            return output.str();
        }
        
        
    };
        
}

#endif
