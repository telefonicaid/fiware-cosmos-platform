
#ifndef _H_STREAM_OPERATION
#define _H_STREAM_OPERATION

/* ****************************************************************************
 *
 * FILE                      StreamOperation.h
 *
 * AUTHOR                    Andreu Urruela Planas
 *
 * Definition of the stream operation for automatic queue processing
 *
 */

#include <sstream>
#include <string>

#include "au/map.h"                         // au::map
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
            
            StreamManager *streamManager;   // Pointer to the stream manager to check ready
            
            std::string last_review;        // Comment from the last review
            
        public:
            
            std::string name;
            std::string operation;
            
            
            std::vector<std::string> input_queues;
            std::vector<std::string> output_queues;
            
            au::Environment environment;

            // Information about activity
            int num_operations;
            int num_blocks;
            size_t size;
            FullKVInfo info;
            int update_state_counter;
            
            // List of current tasks running here
            std::set< QueueTask* > running_tasks;
            
        public:
            
            StreamOperation( StreamManager *streamManager );
            StreamOperation( StreamOperation* streamOperation );
            
            // Instruction to add or remove a particular task for this automatic rule
            void add( QueueTask* task );
            void remove( QueueTask* task );
            
            void add_update_state();
            
            void getInfo( std::ostringstream &output );            
            std::string getStatus();
            
            
            void setActive( bool _active );
            
            bool isActive();

            bool isPaused();
            
            void setNumWorkers( int num_workers );
            int getNumWorkers();
            
            bool isValid();
            
            bool ready( );
            bool compare( StreamOperation *other_stream_operation );
            
        private:
            
            void init();
            
        };
    }
}

#endif