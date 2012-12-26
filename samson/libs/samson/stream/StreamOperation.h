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

#include "au/containers/map.h"                         // au::map
#include "au/string.h"                      // au::Format
#include "au/CommandLine.h"                 // au::CommandLine
#include "au/Environment.h"                 // au::Environment
#include "au/ErrorManager.h"                // au::ErrorManager

#include "engine/Object.h"                  // engine::Object
#include "engine/Object.h"                  // engine::Object
#include "engine/Buffer.h"                  // engine::Buffer

#include "samson/common/Rate.h"
#include "samson/common/samson.pb.h"        // network::...
#include "samson/common/NotificationMessages.h"
#include "samson/common/EnvironmentOperations.h"

#include "samson/module/Environment.h"      // samson::Environment

#include "samson/stream/QueueTaskManager.h" // samson::stream::QueueTaskManager
#include "samson/stream/BlockListContainer.h"       // BlockListContainer ( parent class )
#include "samson/stream/BlockList.h"                // BlockList
#include "samson/stream/DistributionInformation.h"


#include "BlockInfo.h"                              // struct BlockInfo

namespace samson {
    
    class SamsonWorker;
    class Info;
    class Operation;
    
    namespace stream
    {
        
        class Queue;
        class QueueTask;
        class Block;
        class BlockList;
        class WorkerCommand;
        class PopQueue;
        
        
        class StreamOperationBase
        {
            
        public:
            
            std::string name;                           // Name of the operation
            std::string operation;                      // Operation name used in this stream operation
            
            std::vector<std::string> input_queues;      // Input queues
            std::vector<std::string> output_queues;     // Output queues

            DistributionInformation distribution_information;    // Information about how to distribute information
            au::Environment environment;                         // Environment properties ( used to save and restore everything )
            
            StreamOperationBase()
            {
            }
            
            // Only used when running with "run" command
            StreamOperationBase( std::string _operation , DistributionInformation _distribution_information )
            {
                name = "manual";
                operation = _operation;
                distribution_information = _distribution_information;
            }
            
            StreamOperationBase( StreamOperationBase* other )
            {
                name = other->name;
                operation = other->operation;
                
                input_queues.insert( input_queues.begin() , other->input_queues.begin() , other->input_queues.end() );
                output_queues.insert( output_queues.begin() , other->output_queues.begin() , other->output_queues.end() );
                
                distribution_information = other->distribution_information;
                
                // Copy environment properties
                environment.copyFrom( &other->environment );
            }
            
        };
        
        class StreamOperation : public StreamOperationBase , public BlockListContainer 
        {
            friend class StreamManager;
            
        protected:
            
            // Get time statistics for this stream operation
            au::map< Block*, au::Cronometer > block_cronometers;
            double latency_time;

            void add_latency_time( double t );
            
            void add_block_to_cronometers( Block* block );
            void remove_block_to_cronometers( Block* block );

            void add_block_to_cronometers( BlockList* block_list );
            void remove_block_to_cronometers( BlockList* block_list );
            
            void add_block_to_cronometers( BlockListContainer * block_list );
            void remove_block_to_cronometers( BlockListContainer* block_list );
            
            
        protected:
            
            StreamManager *streamManager;   // Pointer to the stream manager to check ready

            std::string command;            // Original command that originated this StreamOperation
            std::string last_review;        // Comment from the last review
            
            ::samson::Rate input_rate;      // Input and Output rate for first channel
            ::samson::Rate output_rate;

            size_t in_num_operations;       // Input and outut number of operations
            size_t out_num_operations;
            
            size_t out_core_seconds;        // Core seconds spent at this stream operation
            
        public:

            virtual ~StreamOperation(){}

            // Total input history
            BlockInfo history_block_info;   // Historical information
            
            //FullKVInfo info;
            //size_t temporal_size;
            
            // List of current tasks running here
            std::set< QueueTask* > running_tasks;

            // Private constructor
            StreamOperation( );
            
            
            virtual std::string getStatus()
            {
                return "( no status )";
            }
            
        public:
            
            // Static function to get a new Stream Operation
            static StreamOperation* newStreamOperation( StreamManager *streamManager , std::string command, au::ErrorManager& error );
            
            // Instruction to add or remove a particular task for this automatic rule
            void add( QueueTask* task );
            void remove( QueueTask* task );
            
            void getInfo( std::ostringstream &output );            
            
            void setPaused( bool _active );
            bool isPaused();
            
            virtual bool isValid()
            {
                LM_X(1,("Internal error")); // This should be always subclased
                return false;
            }
            
            Operation* getOperation();
            
            int getPriority();
            
            virtual void push( BlockList *list )=0;
            
            int getCoreSeconds()
            {
                return environment.getInt("system.core_seconds" , 0 );
            }
            
            int getMaxTasks()
            {
                return environment.getInt("system.max_tasks" , 0 );
            }
            
            // Option to create collections
            void fill( samson::network::CollectionRecord* record , VisualitzationOptions options );
            
            
        private:
            
            // ------------------------------------------------------------------------------------
            // Scheduling new tasks
            // ------------------------------------------------------------------------------------
            
            // Function to know if there are new tasks to be scheduled and the priority of this next task
            // Return "0" means "no task to be scheduled"
            virtual size_t getNextQueueTaskPriorityParameter( )=0;
            
            // Function to schedule one or more tasks ( if we were the higher priority operation )
            virtual void scheduleNextQueueTasks( )=0;
            
        };
        
        
// Spetialized version of StreamOperation
        
        class StreamOperationForward : public StreamOperation
        {
          
            au::Cronometer cronometer;  // Cronometer of the last execution
            
        public:
            
            bool isValid();
            void push( BlockList *list );
            
            virtual std::string getStatus()
            {
                std::ostringstream output;
                output << "( ";
                output << "Forward operation.";
                size_t num_tasks = running_tasks.size();
                if( num_tasks > 0 )
                    output << "Running " << num_tasks << " tasks";
                
                int max_tasks = getMaxTasks();
                if ( max_tasks > 0 )
                    output << "Maximum " << max_tasks << " tasks";
                output << ") ";
                return output.str();
            }
            
            
        private:

            // Priority for the next tasks
            size_t getNextQueueTaskPriorityParameter( );
            
            // Scheduling new tasks
            void scheduleNextQueueTasks( );

        };

        
        class StreamOperationUpdateState : public StreamOperation
        {
            // Flag to only process states with new inputs
            bool update_only;
            
            // Information to keep track of everything
            int num_divisions;            // Number of divisions  ( state is divided and updated individually in divisions )
            bool *updating_division;      // Vector indicating if we are updating a particular division ( not possible to update this division in paralel )

            int numUpdatingDivisions();            // Number of divisions currently being updated
            int next_division_to_be_updated();     // Next division to be updated ( if any ) Criteria: maximum input and non being updated. 
            
        public:
            
            StreamOperationUpdateState( int _num_divisions, bool _update_only );
            ~StreamOperationUpdateState();
            
            virtual std::string getStatus()
            {
                std::ostringstream output;
                output << "( ";
                output << "Update state with " << num_divisions << " division.";
                size_t num_tasks = running_tasks.size();
                if( num_tasks > 0 )
                    output << "Running " << num_tasks << " tasks.";
                
                int max_tasks = getMaxTasks();
                if ( max_tasks > 0 )
                    output << "Maximum " << max_tasks << " tasks.";
                
                output << "Maximum state size " << au::str( getMaxStateSize() ) << ".";
                
                output << ") ";
                return output.str();
            }
            
            
            bool isValid();
            void push( BlockList *list );
            
            void finish_update_division( int division )
            {
                if( !updating_division[ division ] )
                    LM_W(("Update division finish without being locked... this should be an error"));
                updating_division[ division ] = false;
            }

        private:
            
            // Priority for the next tasks
            size_t getNextQueueTaskPriorityParameter( );
            
            // Scheduling new tasks
            void scheduleNextQueueTasks( );
            
            // Auxiliar function
            void getAllInputBlocks( BlockList *blockList );
            size_t getMaxStateSize();
            
        };

        class StreamOperationForwardReduce : public StreamOperation
        {
            
        public:
            
            bool isValid();
            void push( BlockList *list );

            virtual std::string getStatus()
            {
                std::ostringstream output;
                output << "( ";
                output << "Forward reduce operation.";
                size_t num_tasks = running_tasks.size();
                if( num_tasks > 0 )
                    output << "Running " << num_tasks << " tasks";
                
                int max_tasks = getMaxTasks();
                if ( max_tasks > 0 )
                    output << "Maximum " << max_tasks << " tasks";
                output << ") ";
                return output.str();
            }            
            
        private:
            
            // Priority for the next tasks
            size_t getNextQueueTaskPriorityParameter( );
            
            // Scheduling new tasks
            void scheduleNextQueueTasks( );
            
        };        
        
        
    }
}

#endif