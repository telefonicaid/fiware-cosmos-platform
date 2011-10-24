
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
#include "au/ErrorManager.h"                // au::ErrorManager

#include "engine/Object.h"                  // engine::Object
#include "engine/Object.h"                  // engine::Object
#include "engine/Buffer.h"                  // engine::Buffer

#include "samson/common/samson.pb.h"        // network::...
#include "samson/common/NotificationMessages.h"
#include "samson/module/Environment.h"      // samson::Environment
#include "samson/common/EnvironmentOperations.h"

#include "samson/stream/QueueTaskManager.h" // samson::stream::QueueTaskManager


#include "samson/stream/BlockListContainer.h"       // BlockListContainer ( parent class )
#include "samson/stream/BlockList.h"                // BlockList

#include "samson/data/SimpleDataManager.h"          // samson::SimpleDataManager


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

            int num_workers;                            // number of workers
            
            StreamOperationBase()
            {
                
            }
            
            // Only used when running run_stream_operation
            StreamOperationBase( std::string _operation , int _num_workers )
            {
                name = "manual";
                operation = _operation;
                
                num_workers = _num_workers;
            }
            
            StreamOperationBase( StreamOperationBase* other )
            {
                name = other->name;
                operation = other->operation;
                
                input_queues.insert( input_queues.begin() , other->input_queues.begin() , other->input_queues.end() );
                output_queues.insert( output_queues.begin() , other->output_queues.begin() , other->output_queues.end() );
                
                num_workers = other->num_workers;
            }
            
        };
        
        class StreamOperation : public StreamOperationBase , public BlockListContainer 
        {
            friend class StreamManager;
            
        protected:
            
            StreamManager *streamManager;   // Pointer to the stream manager to check ready
            std::string last_review;        // Comment from the last review
            std::string command;            // Original command that originated this StreamOperation
            
        public:
            
            au::Environment environment;                // Environment properties ( used to save and restore everything )

            // Information about activity ( temporal counters )
            
            int num_operations;
            int num_blocks;
            size_t temporal_size;
            FullKVInfo info;
            int temporal_core_seconds;
            
            // List of current tasks running here
            std::set< QueueTask* > running_tasks;

            // Private constructor
            StreamOperation( );
            
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
            
            virtual void review()
            {
                LM_X(1,("Internal error")); // This should be always subclased
            }
            
            virtual bool scheduleNextQueueTasks( )
            {
                LM_X(1,("Internal error")); // This should be always subclased
                return false;
            }
            
            virtual std::string getStatus()
            {
                return "Non implemented getStatus() at StreamOperation";
            }
            
            int getCoreSeconds()
            {
                return environment.getInt("system.core_seconds" , 0 );
            }
            
        };
        
        
// Spetialized version of StreamOperation
        
        class StreamOperationForward : public StreamOperation
        {
            
        public:
            
        public:
            
            bool isValid();
            void review();
            virtual bool scheduleNextQueueTasks( );
            std::string getStatus();

        };

        
        class StreamOperationUpdateState : public StreamOperation
        {
            
            // Information to keep track of everything
            int num_divisions;                  // Global number of divisions we are considering ( taking the size of the state )
            bool *updating_division;            // Vector indicating if we are updating a particular divisions

            int numUpdatingDivisions();            
            int max_size_division();            
            int getMinNumDivisions();
            
        public:
            
            StreamOperationUpdateState();
            ~StreamOperationUpdateState();
            
            bool isValid();
            void review();
            virtual bool scheduleNextQueueTasks( );
            std::string getStatus();
            
            void finish_update_division( int division )
            {
                if( !updating_division[ division ] )
                    LM_W(("Update division finish without being locked... this should be an error"));
                
                updating_division[ division ] = false;
            }
            
            
            void transform_block_lists( int previous_num_division , int future_num_division )
            {
                if( num_divisions != previous_num_division )
                    LM_X(1,("Internal error"));

                if( numUpdatingDivisions() != 0 )
                    LM_X(1,("Internal error"));
                
                for (int i = 0 ; i < future_num_division ; i++ )
                    updating_division[i] = false;
                
                BlockListContainer auxBlockListContainer;
                auxBlockListContainer.copyFrom( this );
                clearBlockListcontainer();

                int factor = future_num_division / previous_num_division;
                
                for ( int i = 0 ; i < previous_num_division ; i++ )
                {
                    for ( int f = 0 ; f < factor ; f++ )
                    {
                        // Old queue i 
                        BlockList *oldQueue = auxBlockListContainer.getBlockList( au::str("input_%d", i ) );

                        // New queue i*factor + f
                        BlockList *newQueue = getBlockList( au::str("input_%d", i*factor + f ) );
                        
                        // Copy content
                        newQueue->copyFrom( oldQueue );
                    }
                }
                
                num_divisions = future_num_division;
                
            }
            
            
            void getAllInputBlocks( BlockList *blockList );
            
        };
        
        
    }
}

#endif