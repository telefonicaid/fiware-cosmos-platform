/*
 * Telefónica Digital - Product Development and Innovation
 *
 * THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND,
 * EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
 *
 * Copyright (c) Telefónica Investigación y Desarrollo S.A.U.
 * All rights reserved.
 */
#ifndef _H_STREAM_WORKER_COMMAND
#define _H_STREAM_WORKER_COMMAND

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

#include "au/containers/map.h"                         // au::map
#include "au/containers/vector.h"
#include "au/string.h"                      // au::Format
#include "au/CommandLine.h"                 // au::CommandLine

#include "engine/Object.h"                  // engine::Object

#include "samson/common/Visualitzation.h"
#include "samson/common/samson.pb.h"        // network::...
#include "samson/module/Environment.h"      // samson::Environment
#include "samson/module/ModulesManager.h"   // samson::ModulesManager

#include "samson/stream/QueueTaskManager.h" // samson::stream::QueueTaskManager
#include "samson/stream/StreamManager.h" 

#include "engine/Object.h"                  // engine::Object
#include "engine/Buffer.h"                  // engine::Buffer
#include "engine/BufferContainer.h"

namespace samson {
    
    class SamsonWorker;
    class Info;
    
    
    class Queue;
    class Block;
    class BlockList;
    namespace stream {
        class StreamOperationBase;
    }
    
    // Worker Tasks is an action working on this worker
    // It is basically a set of individual queue-tasks
    
    class WorkerCommand : public engine::Object
    {
        std::string worker_command_id; // Unique identifier ( used to associate all items associated with this worker_command )
        
        bool notify_finish;                                         // Flag to mark if it is necessary to notify when finish
        network::WorkerCommand *originalWorkerCommand;              // Copy of the original message
        
        // Identifiers to notify when finished
        size_t delilah_id;                                          // Delilah identifier of this task   
        size_t delilah_component_id;                                // Identifier inside delilah
        
        // Pointer to the samsonWorker and streamManager to interact with everything inside this worker
        SamsonWorker * samsonWorker;
        stream::StreamManager* streamManager;
        
        // Error management
        au::ErrorManager error;
        
        // Command to run
        std::string command;
        
        // Environment properties
        Environment enviroment;
        
        // Flag to flush queues at the end of the operation
        bool flush_queues;
        
        // Flag to indicate that this command is still pending to be executed
        bool pending_to_be_executed;
        
        // Flag to indicate that this worker-command has been completed and a message has been sent back to who sent the request
        bool finished;
        
        // Number of pending processes
        int num_pending_processes;
        int num_pending_disk_operations;
        
        // Collections added in the response message
        au::vector< samson::network::Collection > collections;
        
        // BufferContainer to hold the buffer to send back to the delilah client
        engine::BufferContainer buffer_container;
        
        friend class WorkerCommandManager;
        
    public:
        
        WorkerCommand( std::string worker_command_id ,size_t _delilah_id , size_t _delilah_component_id , const network::WorkerCommand& _command );
        ~WorkerCommand();
        
        // Handy function to set the internal buffer in buffer_container
        void setBuffer( engine::Buffer * buffer );
        
        void setSamsonWorker( SamsonWorker * _samsonWorker );
        bool isFinished();
        
        void runCommand( std::string command , au::ErrorManager* error );
        void run();
        
        void notify( engine::Notification* notification );

        // XML report
        void getInfo( std::ostringstream& output);

        // Fill a collection record
        void fill( samson::network::CollectionRecord* record , Visualization* visualization );
        
    private:
        
        void finishWorkerTaskWithError( std::string error_message );
        void finishWorkerTask();
        
        // Auxiliar operation    
        stream::StreamOperationBase *getStreamOperation( Operation *op );
        
        // Create collection for buffers (Memory manager )
        network::Collection* getCollectionOfBuffers( Visualization* visualization );

        
        // Function to check everything is finished
        void checkFinish();
        
        
        
    };
}

#endif