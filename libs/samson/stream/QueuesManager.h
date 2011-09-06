

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

#include <sstream>
#include <string>

#include "au/map.h"     // au::map
#include "au/string.h"  // au::Format

#include "samson/common/samson.pb.h"        // network::...
#include "samson/stream/QueueTaskManager.h" // samson::stream::QueueTaskManager

#include "engine/Object.h"                  // engine::Object
#include "engine/Buffer.h"                  // engine::Buffer

#include "samson/stream/StreamOperation.h"  // Operation
#include "samson/stream/PopQueueManager.h"  // PopQueueManager

namespace samson {
    
    class SamsonWorker;
    class Info;
    
    namespace stream
    {
        
        class Queue;
        class Block;
        class BlockList;
            
        class QueuesManager
        {
            
            friend class Queue;
            
            // Map with the current queues
            au::map< std::string , Queue > queues;                

            // list of automatic operation ( updated from controller )
            network::StreamOperationList *operation_list;    

            // Manager of the tasks associated with the queues
            QueueTaskManager queueTaskManager;      
            
            // Manager for the pop queue operations
            PopQueueManager popQueueManager;        
            
            // Pointer to the controller to send messages
            ::samson::SamsonWorker* worker;         
            
        public:
            
            QueuesManager( ::samson::SamsonWorker* worker );

            // Add a block to a particular queue ( ususally from the network interface )
            void addBlocks( std::string queue_name ,  BlockList *bl );
            
            // set list of automatic operations( from controller )
            void setOperationList( network::StreamOperationList *list );
                        
            // Remove a particular queue or state
            
            void remove_queue( std::string queue_name );
            
            // Pause and Play a particular state
            void pause_queue( std::string queue_name );
            void play_queue( std::string queue_name );
            
            // Notify finish task
            void notifyFinishTask( QueueTask *task );
            void notifyFinishTask( PopQueueTask *task );
            
            // Add a pop queue operation
            void addPopQueue(const network::PopQueue& pq , size_t delilahId, int fromId );
            
            // Get information for monitorization
            void getInfo( std::ostringstream& output);
            
        private:
            
            Queue* getQueue( std::string name );
            
            void reviewStreamOperations();
            void reviewStreamOperation(const network::StreamOperation& operation);
            
        };
    }
}

#endif
