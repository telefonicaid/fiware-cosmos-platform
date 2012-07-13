

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

#include "au/containers/map.h"                         // au::map
#include "au/containers/vector.h"
#include "au/string.h"                      // au::Format
#include "au/CommandLine.h"                 // au::CommandLine

#include "au/network/Service.h"
#include "au/network/RESTService.h"

#include "engine/Object.h"                  // engine::Object
#include "engine/Object.h"                  // engine::Object
#include "engine/Buffer.h"                  // engine::Buffer

#include "samson/common/samson.pb.h"        // network::...
#include "samson/common/status.h" 

#include "samson/common/NotificationMessages.h"
#include "samson/module/Environment.h"      // samson::Environment
#include "samson/common/EnvironmentOperations.h"

#include "samson/stream/QueueTaskManager.h" // samson::stream::QueueTaskManager
#include "samson/stream/QueueConnections.h"

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

            // Map of quuee connections
            au::map <std::string , QueueConnections > queue_connections;
            
            
            // Cronometer since the last "save state to disk"
            au::Cronometer cronometer_save_state_to_disk;
            
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
            ~StreamManager();
            
            
            // ------------------------------------------------------------
            // Operations over queues
            // ------------------------------------------------------------
            
            // Add a buffer to a particular queue ( ususally from the network interface )
            void addBuffer( std::string queue_name ,  engine::Buffer *buffer );
                        
            // Remove a particular queue or state
            void remove_queue( std::string queue_name );

            // Copy contents of a queue to another queue
            void push_queue( std::string from_queue_name , std::string to_queue_name );
            
            // Flush accumulated buffers in all queues ( necessary when a particular operation has finished)
            void flushBuffers();
            
            // ------------------------------------------------------------
            // pop and stream out from queue 
            // ------------------------------------------------------------
            
            // Add a pop queue operation
            void addPopQueue(const network::PopQueue& pq , size_t delilah_id, size_t delilah_component );
            
            // Connect and disconnect to a queue
            void connect_to_queue( size_t fromId , std::string queue , bool flag_new , bool flag_remove );
            void disconnect_from_queue( size_t fromId , std::string queue );

            // Notification of a disconnected delilah
            void notify_delilah_disconnection( size_t delilah_id );
            
            // ------------------------------------------------------------
            // Operations over stream operations
            // ------------------------------------------------------------
            
            void add( StreamOperation* operation );
            Status remove_stream_operation( std::string name );
            Status set_stream_operation_property( std::string name , std::string property, std::string value );
            Status unset_stream_operation_property( std::string name , std::string property );
            StreamOperation* getStreamOperation( std::string name );
            KVFormat expected_format_for_queue( std::string queue );

            // ------------------------------------------------------------
            // Operations over queue connections
            // ------------------------------------------------------------
            
            void add_queue_connection( std::string source_queue , std::string target_queue );
            void remove_queue_connection( std::string source_queue , std::string target_queue );

            
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
            samson::network::Collection* getCollection( Visualization* visualizaton );
            samson::network::Collection* getCollectionForStreamOperations(VisualitzationOptions options ,  std::string pattern );
            samson::network::Collection* getCollectionForQueueConnections( Visualization* visualizaton );
            samson::network::Collection* getCollectionForStreamBlock( std::string path , Visualization* visualizaton );
            samson::network::Collection* getCollectionForPopConnections( Visualization* visualizaton );
            
            
            // Get a pointer to a particular queue
            Queue* getQueue( std::string name );
            
            // Get value for a particular state
            void process( au::network::RESTServiceCommand* command );
            
            // check if something is pending to be processed ( only for wait command in tests )
            bool isSomethingPending();
            
        private:
            
            void reviewStreamOperations();
            void review_connections();
            void saveStateToDisk();
            void recoverStateFromDisk();
            
            // Recover the stream operations for a particular queue
            au::vector<StreamOperation> getStreamOperationsForQueues( std::string queue );

            
        };
    }
}

#endif
