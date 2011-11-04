

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
#include "samson/common/EnvironmentOperations.h"

#include "samson/stream/QueueTaskManager.h" // samson::stream::QueueTaskManager


#include "samson/data/SimpleDataManager.h"          // samson::SimpleDataManager


namespace samson {
    
    class SamsonWorker;
    class Info;
    
    namespace stream
    {
        
        class Queue;
        class QueueTask;
        class Block;
        class BlockList;
        class WorkerCommand;
        class PopQueue;
        class StreamOperation;
        class StreamOutConnection;
        
        class StreamManager : public ::engine::Object 
        {
            
            friend class Queue;
            friend class QueueItem;
            friend class WorkerCommand;
            friend class BlockBreakQueueTask;
            friend class ReduceQueueTask;
            friend class StreamOperation;
            friend class StreamOperationForward;
            friend class StreamOperationUpdateState;
            friend class StreamOperationForwardReduce;
            friend class StreamOutConnection;
            
            // Map with the current queues
            au::map< std::string , Queue > queues;                

            // Map of stream operaitons
            au::map <std::string , StreamOperation> stream_operations;

            // Manager of the tasks associated with the queues
            QueueTaskManager queueTaskManager;      
            
            // Pop queue operations
            size_t id_pop_queue;                        // Identifier to the new pop queue operations
            au::map< size_t , PopQueue > popQueues;     // Map of current pop-queue operations
            
            // Pointer to the controller to send messages
            ::samson::SamsonWorker* worker;         

            // Manager of the current "stream-tasks" running on this worker
            au::map< size_t , WorkerCommand > workerCommands; 
            
            // StreamOutConnection ( connection to receive data from queues in stream mode )
            au::map< int , StreamOutConnection > stream_out_connections;
            
            // Internal counter to WorkerTasks
            size_t worker_task_id;

            
        public:
            
            StreamManager( ::samson::SamsonWorker* worker );

            // Add a block to a particular queue ( ususally from the network interface )
            void addBlocks( std::string queue_name ,  BlockList *bl );
            
            // set list of automatic operations( from controller )
            void add( StreamOperation* operation );
            
            // Add a worker command to this stream manager
            void addWorkerCommand( WorkerCommand *workerCommand );
            
            // Remove a particular queue or state
            void remove_queue( std::string queue_name );

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

            Queue* getQueue( std::string name );

            StreamOperation* getStreamOperation( std::string name );
            
            // Connect and disconnect to a queue
            void connect_to_queue( int fromId , std::string queue );
            void disconnect_from_queue( int fromId , std::string queue );

            // Reset all the content of this stream manager
            void reset();
            
            // Get a new id for the next queue task operation
            size_t getNewId()
            {
                return queueTaskManager.getNewId();
            }

            
        private:
            
            void reviewStreamOperations();
            
            void saveStateToDisk();
            void recoverStateFromDisk();

            
        };
    }
}

#endif
