

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
#include "samson/common/samson.pb.h"        // network::...
#include "samson/stream/QueueTaskManager.h" // samson::stream::QueueTaskManager
#include "engine/EngineNotification.h"            // engine::NotificationListener


namespace samson {
    
    class SamsonWorker;
    
    namespace stream
    {
        
        class Queue;
        class Block;
        
        class QueuesManager
        {
            au::map< std::string , Queue > queues;  // Map with the current queues
            
            QueueTaskManager queueTaskManager;      // Manager of the tasks associated with the queues
            
            friend class Queue;
            
            ::samson::SamsonWorker* worker;       // Pointer to the controller to send messages
            
        public:
            
            QueuesManager( ::samson::SamsonWorker* worker );
            std::string getStatus();

            // Add a block to a particular queue ( ususally from the network interface )
            void addBlock( std::string queue , int channel , Block *b);
            
            // set info ( from controller )
            void setInfo( network::StreamQueue &queue );

            // Notify finish task
            void notifyFinishTask( std::string queue , size_t task );
            
        private:
            
            Queue* getQueue( std::string name );
        };
    }
}

#endif
