

#ifndef _H_STREAM_OUTPUT
#define _H_STREAM_OUTPUT

/* ****************************************************************************
 *
 * FILE                      StreamOutput.h
 *
 * AUTHOR                    Andreu Urruela Planas
 *
 * Manager for the stream connections
 *
 */

#include <sstream>
#include <string>
#include <set>

#include "au/containers/map.h"                         // au::map
#include "au/string.h"                      // au::Format
#include "au/CommandLine.h"                 // au::CommandLine
#include "au/Environment.h"                 // au::Environment

#include "engine/Object.h"                  // engine::Object
#include "engine/Object.h"                  // engine::Object
#include "engine/Buffer.h"                  // engine::Buffer

#include "samson/common/samson.pb.h"        // network::...
#include "samson/common/NotificationMessages.h"
#include "samson/module/Environment.h"      // samson::Environment
#include "samson/common/EnvironmentOperations.h"

#include "samson/stream/QueueTaskManager.h" // samson::stream::QueueTaskManager




namespace samson {
    
    class SamsonWorker;
    class Info;
    
    namespace stream
    {
        
        class BlockList;
        class StreamOutQueueTask;
        class StreamManager;
        
        class StreamOutQueue
        {
            std::string queue;              // Name of the queue we want to receive data
            BlockList* list;                // List of blocks that should be flush to this client
            
            
            bool flag_remove;
            
            // au::Environment environment;    // Environment properties ( if necessary in the future )
            
            public :
            
            StreamOutQueue( std::string _queue ,bool _flag_remove );
            ~StreamOutQueue( );
            
            void push( BlockList* list );
            bool extractBlockFrom( BlockList *list );            

            // Get XML monitoring information
            void getInfo( std::ostringstream& output)
            {
                au::xml_open(output , "stream_out_queue"); 
                
                au::xml_simple( output , "queue" , queue );
                
                au::xml_single_element(output, "block_list", list );
                
                au::xml_close(output , "stream_out_queue"); 
            }
            
            std::string str()
            {
                return au::str("%s [%s]" , queue.c_str() , au::str( list->getBlockInfo().info.size , "B" ).c_str() );
            }
            
            
        };
        
        class StreamOutConnection
        {
            size_t fromId;                                                       // Identifier of this element ( network library )
            
            au::map<std::string,StreamOutQueue> stream_out_queues;               // List of queues you are connected to
            
            std::set< StreamOutQueueTask* > running_stream_out_queue_tasks;      // List of running tasks
            
            int max_num_stream_out_queue_tasks;                                  // Limit of simultaneous stream_queue_tasks
            
            StreamManager *streamManager;   // Pointer to the global stream manager                                     
            
            
        public:
            
            StreamOutConnection( StreamManager *_streamManager , size_t _fromId  );
            ~StreamOutConnection();
            
            // Push content if this queue if really connected
            void push( std::string queue , BlockList* list );

            // Add a queue for this user
            StreamOutQueue* add_queue( std::string queue ,bool _flag_remove );
            void remove_queue( std::string queue );
            
            // Get the next task to be executed
            void scheduleNextTasks();
            
            // Get XML monitoring information
            void getInfo( std::ostringstream& output);
            
            // Fill with records for monitorization
            void fill( samson::network::CollectionRecord* record , Visualization* visualization );
                
        };

    }
}

#endif
