/*
 * Telefónica Digital - Product Development and Innovation
 *
 * THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND,
 * EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
 *
 * Copyright (c) Telefónica Investigación y Desarrollo S.A.U.
 * All rights reserved.
 */



#ifndef _H_SAMSON_STREAM_QUEUE_CONNECTIONS
#define _H_SAMSON_STREAM_QUEUE_CONNECTIONS

#include <string>                       // std::string

#include "au/containers/list.h"                    // au::list
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
        
        void fill( samson::network::CollectionRecord* record , Visualization* visualization )
        {
            if (visualization == NULL)
                LM_D(("visualization == NULL"));

            add( record , "queue" , queue , "left,different" );
            add( record , "connected to.." , str_target_queues() , "left,different" );
        }
        
        std::vector<std::string> getTargetQueues()
        {
            std::vector<std::string> queues;
            std::set<std::string>::iterator it_target_queues;
            for ( it_target_queues = target_queues.begin() ; it_target_queues != target_queues.end() ; it_target_queues++  )
                queues.push_back( *it_target_queues );
            return queues;
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
