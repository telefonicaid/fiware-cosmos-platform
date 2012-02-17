

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
#include "samson/common/status.h" 

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

        public:
            
            // Manager of the tasks associated with the queues
            QueueTaskManager queueTaskManager;      

        private:
            
            // Pop queue operations
            size_t id_pop_queue;                        // Identifier to the new pop queue operations
            au::map< size_t , PopQueue > popQueues;     // Map of current pop-queue operations
            
            // Pointer to the controller to send messages
            ::samson::SamsonWorker* worker;         

            // StreamOutConnection ( connection to receive data from queues in stream mode from delilahs )
            au::map< size_t , StreamOutConnection > stream_out_connections;
            
            
            //Save state stuff
            bool currently_saving;              // Flag indicating if we are currently writing state to disk
            
        public:
            
            StreamManager( ::samson::SamsonWorker* worker );

            // ------------------------------------------------------------
            // Operations over queues
            // ------------------------------------------------------------
            
            // Add a block to a particular queue ( ususally from the network interface )
            void addBlocks( std::string queue_name ,  BlockList *bl );
            
            // Remove a particular queue or state
            void remove_queue( std::string queue_name );

            // Copy contents of a queue to another queue
            void cp_queue( std::string from_queue_name , std::string to_queue_name );
            
            // ------------------------------------------------------------
            // pop queue operations
            // ------------------------------------------------------------
            
            // Add a pop queue operation
            void addPopQueue(const network::PopQueue& pq , size_t delilah_id, size_t delilah_component );
            
            // Connect and disconnect to a queue
            void connect_to_queue( size_t fromId , std::string queue , bool flag_new , bool flag_remove );
            void disconnect_from_queue( size_t fromId , std::string queue );

            // ------------------------------------------------------------
            // Operations over stream operations
            // ------------------------------------------------------------
            
            void add( StreamOperation* operation );
            Status remove_stream_operation( std::string name );
            Status set_stream_operation_property( std::string name , std::string property, std::string value );
            StreamOperation* getStreamOperation( std::string name );
            
            // Get information for monitoring
            void getInfo( std::ostringstream& output);
            
            // Engine notification function
            void notify( engine::Notification* notification );
            
            // Reset all the content of this stream manager
            void reset();
            
            // Get a new id for the next queue task operation
            size_t getNewId()
            {
                return queueTaskManager.getNewId();
            }
            
            // Notify finish task
            void notifyFinishTask( QueueTask *task );
            void notifyFinishTask( SystemQueueTask *task );

            // Get collections from a command ( ls_queue , .... )
            samson::network::Collection* getCollection(VisualitzationOptions options ,  std::string pattern );
            
            // Get a pointer to a particular queue
            Queue* getQueue( std::string name );
            
            
            // Get value for a particular state
            std::string getState( std::string queue_name , const char * key );
            
        private:
            
            void reviewStreamOperations();
            
            void saveStateToDisk();
            void recoverStateFromDisk();

            
        };
    }
}

#endif
