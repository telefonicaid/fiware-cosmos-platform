

#ifndef _H_STREAM_QUEUE_MANAGER
#define _H_STREAM_QUEUE_MANAGER

/* ****************************************************************************
 *
 * FILE                      StreamManager.h
 *
 * AUTHOR                    Andreu Urruela Planas
 *
 * All the queues contained in the system
 *
 */

#include <sstream>
#include <string>

#include "au/map.h"                         // au::map
#include "au/string.h"                      // au::Format
#include "au/CommandLine.h"                 // au::CommandLine

#include "engine/Object.h"                  // engine::Object
#include "engine/Object.h"                  // engine::Object
#include "engine/Buffer.h"                  // engine::Buffer

#include "samson/common/samson.pb.h"        // network::...
#include "samson/common/NotificationMessages.h"
#include "samson/module/Environment.h"      // samson::Environment

#include "samson/stream/QueueTaskManager.h" // samson::stream::QueueTaskManager


#include "samson/data/SimpleDataManager.h"          // samson::SimpleDataManager

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
        class WorkerCommand;
        class BlockBreak;

        
        class StreamManager : public ::engine::Object 
        {
            
            friend class Queue;
            friend class QueueItem;
            friend class WorkerCommand;
            friend class BlockBreakQueueTask;
            
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

            // Manager of the current "stream-tasks" running on this worker
            au::map< size_t , WorkerCommand > workerCommands; 

            // Map of how a particular block is divided in smaller blocks
            au::map< size_t , BlockBreak > blockBreaks;
            
            // Internal counter to WorkerTasks
            size_t worker_task_id;

            
        public:
            
            StreamManager( ::samson::SamsonWorker* worker );

            // Add a block to a particular queue ( ususally from the network interface )
            void addBlocks( std::string queue_name ,  BlockList *bl );
            
            // set list of automatic operations( from controller )
            void setOperationList( network::StreamOperationList *list );
            
            // Add a worker command to this stream manager
            void addWorkerCommand( WorkerCommand *workerCommand );
            
            // Remove a particular queue or state
            void remove_queue( std::string queue_name );
            
            // Pause and Play a particular state
            void pause_queue( std::string queue_name );
            void play_queue( std::string queue_name );

            // Copy contents of a queue to another queue
            void cp_queue( std::string from_queue_name , std::string to_queue_name );
            
            // Notify finish task
            void notifyFinishTask( QueueTask *task );
            void notifyFinishTask( SystemQueueTask *task );
            
            // Add a pop queue operation
            void addPopQueue(const network::PopQueue& pq , size_t delilahId, int fromId );
            
            // Get information for monitorization
            void getInfo( std::ostringstream& output);
            
            // Engine notification function
            void notify( engine::Notification* notification );
            
        private:
            
            Queue* getQueue( std::string name );
            BlockBreak* getBlockBreak( size_t block_id );
            
            void reviewStreamOperations();
            void reviewStreamOperation(const network::StreamOperation& operation);
            
            void saveStateToDisk();
            void recoverStateFromDisk();

            
        };
    }
}

#endif
