#ifndef _H_STREAM_WORKER_TASK
#define _H_STREAM_WORKER_TASK

/* ****************************************************************************
 *
 * FILE                      WorkerCommand.h
 *
 * AUTHOR                    Andreu Urruela Planas
 *
 * 
 *
 */

#include <sstream>
#include <string>

#include "au/map.h"                         // au::map
#include "au/string.h"                      // au::Format
#include "au/CommandLine.h"                 // au::CommandLine

#include "engine/Object.h"                  // engine::Object

#include "samson/common/samson.pb.h"        // network::...
#include "samson/module/Environment.h"      // samson::Environment
#include "samson/module/ModulesManager.h"   // samson::ModulesManager
#include "samson/stream/QueueTaskManager.h" // samson::stream::QueueTaskManager

#include "engine/Object.h"                  // engine::Object
#include "engine/Buffer.h"                  // engine::Buffer

namespace samson {
    
    class SamsonWorker;
    class Info;
    
    namespace stream
    {
        
        class Queue;
        class Block;
        class BlockList;
        class StreamOperationBase;
        
        
        // Worker Tasks is an action working on this worker
        // It is basically a set of individual queue-tasks
        
        class WorkerCommand : public engine::Object
        {
            bool notify_finish;                                         // Flag to mark if it is necessary to notify when finish
            network::WorkerCommand *originalWorkerCommand;              // Copy of the original message
            int fromId;                                                 // Element to be notified
            size_t delilah_id;                                          // Identifier job identifier ( if any )
            
            // Pointer to the stream manager
            StreamManager * streamManager;
            
            // Error management
            au::ErrorManager error;
            
            // Command to run
            std::string command;
            
            // Parsed version of the command
            au::CommandLine cmd;
            
            // Environment properties
            Environment enviroment;
            
            // Flag to indicate that this command is still pending to be executed
            bool pending_to_be_executed;
            
            // Flag to indicate that this worker-task has been completed and a message has been sent back to who sent the request
            bool finished;
            
            // Number of pending processes
            int num_pending_processes;
            
        public:
            
            WorkerCommand( int _fromId , size_t _delilah_id ,  const network::WorkerCommand& _command );
            WorkerCommand( std::string _command );
            ~WorkerCommand();
            
            void setStreamManager( StreamManager * _streamManager );
            bool isFinished();

            void run();

            void notify( engine::Notification* notification );

            void getInfo( std::ostringstream& output);
            
        private:

            void finishWorkerTaskWithError( std::string error_message );
            void finishWorkerTask();

            // Auxiliar operation    
            StreamOperationBase *getStreamOperation( Operation *op );
            
            // Function to check everything is finished
            void checkFinish();
          
            
            
        };
    }
}

#endif