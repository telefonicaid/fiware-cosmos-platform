

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

        class StreamOperation
        {
            
        public:
            
            std::string name;
            std::string operation;

            
            std::vector<std::string> input_queues;
            std::vector<std::string> output_queues;
            
            int num_workers;
            
            Environment environment;
            
            bool active;    // Flag to indicate if this operation is still active
            
            // Information about activity
            int num_operations;
            int num_blocks;
            size_t size;
            FullKVInfo info;
            int update_state_counter;

            // List of current tasks running here
            std::set< QueueTask* > running_tasks;
            
        public:
            
            StreamOperation()
            {
                
            }
            
            StreamOperation( StreamOperation* streamOperation )
            {
                name = streamOperation->name;
                operation = streamOperation->operation;
                num_workers = streamOperation->num_workers;
                
                environment.environment.clear();
                environment.copyFrom( &streamOperation->environment );
                
                input_queues.insert( input_queues.begin(), streamOperation->input_queues.begin() , streamOperation->input_queues.end() );
                output_queues.insert( output_queues.begin(), streamOperation->output_queues.begin() , streamOperation->output_queues.end() );
            }

            
            StreamOperation( const network::StreamOperation& streamOperation)
            {
                update( streamOperation );
                
                // Additional information
                num_operations = 0;
                num_blocks = 0;
                size = 0;
                info.clear();
                
                update_state_counter = 0;

            }
            
            
            void update( const network::StreamOperation& streamOperation )
            {
                // UPdate form controller
                name = streamOperation.name();
                operation = streamOperation.operation();

                input_queues.clear();
                for (int i = 0 ; i < streamOperation.input_queues_size() ; i++ )
                    input_queues.push_back( streamOperation.input_queues(i) );

                output_queues.clear();
                for (int i = 0 ; i < streamOperation.output_queues_size() ; i++ )
                    output_queues.push_back( streamOperation.output_queues(i) );

                
                // Something to remove
                num_workers = streamOperation.num_workers();
                
                // Copy environment
                environment.environment.clear();
                const network::Environment & _environment = streamOperation.environment();
                copyEnviroment( _environment , &environment );
                
                active = true;
            }
            
            void setActive( bool _active )
            {
                active = _active;
            }
            
            // Instruction to add or remove a particular task for this automatic rule
            void add( QueueTask* task );
            void remove( QueueTask* task );
            
            void add_update_state()
            {
                update_state_counter++;
            }
            
            void getInfo( std::ostringstream &output )
            {
                if( !active )
                    return;
                
                au::xml_open(output, "stream_operation");
                au::xml_simple(output, "name", name);
                au::xml_simple(output, "operation", operation);
                
                au::xml_open(output, "inputs");
                for ( size_t i = 0 ; i < input_queues.size() ; i++)
                    output << input_queues[i] << " ";
                au::xml_close(output, "inputs");

                au::xml_open(output, "outputs");
                for ( size_t i = 0 ; i < output_queues.size() ; i++)
                    output << output_queues[i] << " ";
                au::xml_close(output, "outputs");
                
                au::xml_simple(output, "properties", environment.getEnvironmentDescription() );
                
                au::xml_simple(output,"status" , getStatus() );
                
                au::xml_close(output, "stream_operation");
            }
            
            std::string getStatus()
            {
                std::ostringstream output;

                if( running_tasks.size() > 0 )
                    output << "[ Running " << running_tasks.size() << " operations ] ";
                
                if( update_state_counter > 0 )
                    output << "[ Updates states " << update_state_counter << " ] ";
                
                if( num_operations > 0)
                    output << "[ " << num_operations << " ops " << au::str( size , "B" ) << " ( " << au::str( info.kvs , "kvs" ) << " )";
                
                return output.str();
            }
            
            
        };
        
        class StreamManager : public ::engine::Object 
        {
            
            friend class Queue;
            friend class QueueItem;
            friend class WorkerCommand;
            friend class BlockBreakQueueTask;
            friend class ReduceQueueTask;
            
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

            Queue* getQueue( std::string name );

            StreamOperation* getStreamOperation( std::string name );
            
            // Reset all the content of this stream manager
            void reset();
            
        private:
            
            void reviewStreamOperations();
            
            void saveStateToDisk();
            void recoverStateFromDisk();

            
        };
    }
}

#endif
