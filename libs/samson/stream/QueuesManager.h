

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

#include "au/map.h"     // au::map
#include "au/Info.h"     // au::Info
#include "au/Format.h"  // au::Format

#include <string>
#include "samson/common/samson.pb.h"        // network::...
#include "samson/stream/QueueTaskManager.h" // samson::stream::QueueTaskManager
#include "engine/Object.h"                  // engine::Object

#include "samson/stream/PopQueueManager.h"  // PopQueueManager

namespace samson {
    
    class SamsonWorker;
    class Info;
    
    namespace stream
    {
        
        class Queue;
        class Block;
            
        class QueuesManager
        {
            
            friend class Queue;
            
            au::map< std::string , Queue > queues;  // Map with the current queues
            
            QueueTaskManager queueTaskManager;      // Manager of the tasks associated with the queues
            PopQueueManager popQueueManager;        // Manager for the pop queue operations
            
            ::samson::SamsonWorker* worker;         // Pointer to the controller to send messages
            
        public:
            
            QueuesManager( ::samson::SamsonWorker* worker );
            std::string getStatus();

            // Add a block to a particular queue ( ususally from the network interface )
            void addBlock( std::string queue , int channel , Block *b);
            
            // set info ( from controller )
            void setInfo( network::StreamQueue &queue );

            // Notify finish task
            void notifyFinishTask( QueueTask *task );
            
            // Add a pop queue operation
            void addPopQueue(const network::PopQueue& pq , size_t delilahId, int fromId );
            
            // Get information for monitorization
            au::Info* getInfo();
            
        private:
            
            Queue* getQueue( std::string name );
        };
    }
}

#endif
