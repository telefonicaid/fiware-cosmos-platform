
#include <algorithm>

#include "engine/Notification.h"
#include "engine/ProcessManager.h"  


#include "samson/common/EnvironmentOperations.h"        // copyEnvironment
#include "samson/common/SamsonSetup.h"                  // SamsonSetup

#include "samson/network/Packet.h"                      // samson::Packet

#include "samson/worker/SamsonWorker.h"

#include "samson/stream/StreamManager.h"                // samson::stream::StreamManager
#include "samson/stream/Queue.h"
#include "samson/stream/BlockList.h"

#include "samson/stream/QueueTasks.h"

#include "WorkerCommand.h"     // Own interface

namespace samson {
    namespace stream
    {
        WorkerCommand::WorkerCommand( int _fromId , size_t _delilah_id ,  const network::WorkerCommand& _command )
        {
            streamManager = NULL;
            
            //Identifiers to notify when finished
            fromId = _fromId;
            delilah_id = _delilah_id;
            notify_finish = true;
            
            // Copy the original message
            originalWorkerCommand = new network::WorkerCommand();
            originalWorkerCommand->CopyFrom( _command );
            
            // Extract enviroment properties
            copyEnviroment( originalWorkerCommand->environment() , &enviroment );
            
            // Extract command for simplicity
            command = originalWorkerCommand->command();
            
            // Parse command
            cmd.set_flag_boolean("clear_inputs");
            cmd.set_flag_string("stream_operation" , "no_stream_operation");
            cmd.set_flag_string("block_break_mode", "no");
            cmd.set_flag_uint64("min_size", 0);         // Set a minimum size to run ( necessary for automatoc maps / parser / reduce / ....
            cmd.set_flag_uint64("max_latency", 0);         // Set a minimum size to run ( necessary for automatoc maps / parser / reduce / ....
            cmd.set_flag_string("delayed_processing", "yes");         // Set a minimum size to run ( necessary for automatoc maps / parser / reduce / ....
            
            cmd.parse( command );
            
            // Original value for the flags
            pending_to_be_executed =  true;
            finished = false;
            
            // No pending process at the moment
            num_pending_processes = 0;
        }
        
        WorkerCommand::WorkerCommand( std::string _command )
        {
            streamManager = NULL;
            
            // Not necessary to notify
            originalWorkerCommand = NULL;
            notify_finish = false;
            
            // Get directly the command to run
            command = _command;
            
            // Parse command
            cmd.set_flag_boolean("clear_inputs");
            cmd.set_flag_string("stream_operation" , "no_stream_operation");
            cmd.set_flag_string("block_break_mode", "no");
            cmd.set_flag_uint64("min_size", 0);         // Set a minimum size to run ( necessary for automatoc maps / parser / reduce / ....
            cmd.set_flag_uint64("max_latency", 0);         // Set a minimum size to run ( necessary for automatoc maps / parser / reduce / ....
            cmd.set_flag_string("delayed_processing", "yes");         // Set a minimum size to run ( necessary for automatoc maps / parser / reduce / ....
            cmd.parse( command );
            
            // Original value for the falg
            finished = false;
            
            // No pending process at the moment
            num_pending_processes = 0;
        }
        
        WorkerCommand::~WorkerCommand()
        {
            if( originalWorkerCommand )
                delete originalWorkerCommand;
        }
        
        void WorkerCommand::setStreamManager( StreamManager * _streamManager )
        {
            streamManager = _streamManager;
        }
        
        
        bool WorkerCommand::isFinished()
        {
            return finished;
        }
        
        void WorkerCommand::run()
        {
            
            if( !pending_to_be_executed )
                return;
            
            // Not pending any more, exept of something happen...
            pending_to_be_executed = false;
            
            // Main command
            std::string main_command = cmd.get_argument(0);

            
            if( main_command == "wait" )
            {
                // Spetial operation to wait until no activity is present in stream manager
                
                if( streamManager->queueTaskManager.isActive() )
                    pending_to_be_executed = true;
                else
                    finishWorkerTask();
                
                return;
            }
            
            if( main_command == "remove_all_stream" )
            {
                streamManager->reset();
                finishWorkerTask();
                return;
            }
            
            if( main_command == "reload_modules" )
            {
                // Spetial operation to reload modules
                ModulesManager::shared()->reloadModules();
                finishWorkerTask();
                return;
            }
            
            if ( cmd.get_argument(0) == "rm_queue" )
            {
                if( cmd.get_num_arguments() < 2 )
                    finishWorkerTaskWithError( au::str("Not enougth parameters for command %s" , main_command.c_str() ) ); 
                else
                {
                    for ( int i = 1 ; i < cmd.get_num_arguments() ; i++ )
                    {
                        std::string queue_name = cmd.get_argument(i);
                        streamManager->remove_queue(  queue_name );
                    }
                    finishWorkerTask();
                }
                
                return;
            }

            if ( cmd.get_argument(0) == "cp_queue" )
            {
                if( cmd.get_num_arguments() < 3 )
                    finishWorkerTaskWithError( au::str("Not enougth parameters for command %s" , main_command.c_str() ) ); 
                else
                {
                    std::string from_queue_name = cmd.get_argument(1);
                    std::string to_queue_name = cmd.get_argument(2);
                    
                    streamManager->cp_queue(  from_queue_name , to_queue_name );
                    
                    finishWorkerTask();
                }
                
                return;
            }

            if ( cmd.get_argument(0) == "set_queue_property" )
            {
                if( cmd.get_num_arguments() < 4 )
                    finishWorkerTaskWithError( au::str("Not enougth parameters for command %s" , main_command.c_str() ) ); 
                else
                {
                    std::string queue_name = cmd.get_argument(1);
                    std::string property = cmd.get_argument(2);
                    std::string value = cmd.get_argument(3);

                    Queue *queue = streamManager->getQueue( queue_name );
                    queue->setProperty( property , value );
                    
                    finishWorkerTask();
                }
                
                return;
            }            
            
            
            
            if ( cmd.get_argument(0) == "pause_queue" )
            {
                if( cmd.get_num_arguments() < 2 )
                    finishWorkerTaskWithError( au::str("Not enougth parameters for command %s" , main_command.c_str() ) ); 
                else
                {
                    std::string queue_name = cmd.get_argument(1);
                    streamManager->pause_queue( queue_name );
                    finishWorkerTask();
                }
                return;
            }
            
            if ( cmd.get_argument(0) == "play_queue" )
            {
                if( cmd.get_num_arguments() < 2 )
                    finishWorkerTaskWithError( au::str("Not enougth parameters for command %s" , main_command.c_str() ) ); 
                else
                {
                    std::string queue_name = cmd.get_argument(1);
                    streamManager->play_queue( queue_name );
                    finishWorkerTask();
                }
                
                return;
            }

            if( cmd.get_argument(0) == "run_stream_update_state" )
            {
                //LM_M(("Running '%s'", command.c_str() ));

                // Parameters used to thrigger or not the automatic-update
                size_t min_size                 = cmd.get_flag_uint64("min_size");              // Minimum size to run an operation ( if delayed_processing is activated )
                size_t max_latency              = cmd.get_flag_uint64("max_latency");           // Max acceptable time to run an operation
                std::string delayed_processing  = cmd.get_flag_string("delayed_processing");    // Delayed operation "yes" or "no"
                
                bool block_break_mode = (cmd.get_flag_string("block_break_mode") == "yes" );
                
                // Operation size    
                size_t default_size = SamsonSetup::shared()->getUInt64("general.memory") / SamsonSetup::shared()->getUInt64("general.num_processess");
                size_t operation_size = (min_size!=0)?min_size:default_size;
                
                // Spetial command to update a state with a blocking reduce operation
                // Usage: run_stream_update_state operation input state
                
                if( cmd.get_num_arguments() < 4 )
                {
                    finishWorkerTaskWithError( au::str("Not enougth parameters for command %s" , main_command.c_str() ) ); 
                    return;
                }
                
                std::string operation_name = cmd.get_argument(1);
                std::string input_name = cmd.get_argument(2);
                std::string state_name = cmd.get_argument(3);
                
                Operation *op = ModulesManager::shared()->getOperation(  operation_name );
                if( !op )
                {
                    finishWorkerTaskWithError( au::str("Unknown operation %s" , operation_name.c_str() ) ); 
                    return;
                }
                if( ( op->getType() != Operation::reduce ) || (op->getNumInputs() != 2 ) )
                {
                    finishWorkerTaskWithError( au::str("Only reduce operations with 2 inputs for run_stream_update_state operations" ) ); 
                    return;
                }
                if( op->getInputFormat( op->getNumInputs()-1 )  != op->getOutputFormat( op->getNumOutputs()-1 ) )
                {
                    finishWorkerTaskWithError( au::str("%s is not a valid reduce operation since last input and output formats are not the same" ) ); 
                    return;
                }
                
                Queue *state = streamManager->getQueue( state_name );
                Queue *input = streamManager->getQueue( input_name );
                
                // Checking formats for the inputs and state
                KVFormat input_queue_format = input->format;
                KVFormat operation_input_format = op->getInputFormat(0);
                if( input_queue_format != KVFormat("*","*") )
                    if( input_queue_format != operation_input_format )
                    {
                        finishWorkerTaskWithError( au::str("Not valid input format %s != %s" , input_queue_format.str().c_str() , operation_input_format.str().c_str()  ) );
                        return;
                    }

                KVFormat state_queue_format = state->format;
                KVFormat operation_state_format = op->getInputFormat(1);
                if( state_queue_format != KVFormat("*","*") )
                    if( state_queue_format != operation_state_format )
                    {
                        finishWorkerTaskWithError( au::str("Not valid state format %s != %s" , state_queue_format.str().c_str() , operation_state_format.str().c_str()  ) );
                        return;
                    }
                
                
                // Check what operations can be done....
                state->setMinimumNumDivisions();
                int num_divisions = state->num_divisions;      // Divisions used in update-state mode
                
                if( block_break_mode )
                {
                    
                    // Make sure, input is divided at least as state
                    if( input->num_divisions < state->num_divisions)
                        input->setNumDivisions( state->num_divisions );
                    
                }
                
                
                if( !state->isQueueReadyForStateUpdate() )
                {
                    // State is not ready for update operations ( this usually means that state is not divided correctly in independent divisions )
                    //LM_M(("State is not ready for update-state operations"));
                    if( num_pending_processes == 0 )
                        finishWorkerTask();
                    return;
                }
                
                
                if( !block_break_mode )
                {
                    // NON - BLOCK BREAK MODE -
                    
                    // In this case, reduce-update state operations are schedules only if all divisions are ready
                    // First we select all blocks to be processed ( max memory_size/2 )
                    
                    BlockInfo input_block_info = input->list->getBlockInfo();
                    size_t latency = input_block_info.min_time_diff();
                    
                    bool cancel_operation = false;
                    if( input_block_info.size < ( operation_size * SamsonSetup::shared()->getInt("general.num_processess") ) )
                        cancel_operation = true;
                    
                    if( cancel_operation )
                    {
                        if( delayed_processing == "no" )   // No latency option
                            cancel_operation= false;
                        
                        if ( (max_latency > 0) && ( latency > max_latency) )
                            cancel_operation = false;
                    }
                    
                    if( input_block_info.num_blocks == 0)
                        cancel_operation = true;
                    
                    if( !cancel_operation )
                    {
                        
                        if( !state->lockAllDivisions() )
                        {
                            // Not ready for this approach...
                            finishWorkerTask();
                            return;
                        }
                        
                        // Select all possible blocks
                        size_t max_common_size = 0.75*(double)SamsonSetup::shared()->getUInt64("general.memory");
                        BlockList global_inputBlockList;
                        global_inputBlockList.extractFrom(input->list, max_common_size );
                        
                        // remove input from the original queue
                        input->list->remove( &global_inputBlockList );       // Remove input all blocks

                        // Log activity in the worker
                        if( cmd.get_flag_string("stream_operation") != "no_stream_operation" )
                            streamManager->worker->logActivity( au::str("[ %s ] Global update %s %s (%d divs) with %s %s operation %s" , 
                                                                        cmd.get_flag_string("stream_operation").c_str() , 
                                                                        state_name.c_str(),
                                                                        state->list->strShortDescription().c_str(),
                                                                        num_divisions,
                                                                        input_name.c_str(),    
                                                                        global_inputBlockList.strShortDescription().c_str(),
                                                                        operation_name.c_str()  ) );
                        
                        
                        // Schedule operations for all divisions if necessary
                        for (int i = 0 ; i < num_divisions ; i++)
                        {
                            KVRange range = rangeForDivision( i , num_divisions );
                            
                            BlockList inputBlockList;
                            inputBlockList.copyFrom(&global_inputBlockList, range, false, 0); // No limit , no exclusive
                            
                            if( inputBlockList.getNumBlocks() > 0 )        // There is something to process in this division
                            {
                                
                                BlockList stateBlockList("stateBlockList");
                                state->getStateBlocksForDivision( i , & stateBlockList );
                                
                                size_t id = streamManager->queueTaskManager.getNewId();
                                
                                // Log activity in the worker
                                if( cmd.get_flag_string("stream_operation") != "no_stream_operation" )
                                    streamManager->worker->logActivity( au::str("[ %s:%lu ] Update div %d/%d of %s %s with %s %s operation %s" , 
                                                                                cmd.get_flag_string("stream_operation").c_str() , 
                                                                                id,
                                                                                i , 
                                                                                num_divisions ,
                                                                                state_name.c_str(),
                                                                                stateBlockList.strShortDescription().c_str(),
                                                                                input_name.c_str(),    
                                                                                inputBlockList.strShortDescription().c_str(),
                                                                                operation_name.c_str()  ) );
                                
                                
                                
                                
                                network::StreamOperation *operation = getStreamOperation( op );
                                
                                ReduceQueueTask * task = new ReduceQueueTask( id , *operation , range );
                                task->addOutputsForOperation(op);
                                
                                // Spetial flag to indicate update_state mode ( process different output buffers )
                                task->setUpdateStateDivision( i );
                                
                                task->getBlockList("input_0")->copyFrom( &inputBlockList );
                                task->getBlockList("input_1")->copyFrom( &stateBlockList );
                                
                                // Set the working size to get statictics at ProcessManager
                                task->setWorkingSize();
                                
                                // Add me as listener and increase the number of operations to run
                                task->addListenerId( getEngineId() );
                                num_pending_processes++;
                                
                                // Schedule tmp task into QueueTaskManager
                                streamManager->queueTaskManager.add( task );
                                
                                delete operation;                                
                                
                                
                            }
                            else
                            {
                                // There are no operations for this divisions
                                state->unlockDivision( i );
                            }
                            
                        }
                        
                        
                        
                    }
                    
                }
                else
                {
                    // BLOCK BREAK MODE
                    
                    for ( int i = 0 ; i < num_divisions ; i++)
                    {
                        
                        // If not possible 
                        if( !state->lockDivision( i ) )
                        {
                            //LM_M(("Not possible to lock division %d for queue %s" ,  i , state_name.c_str()  )); 
                            continue;
                        }
                        
                        KVRange range = rangeForDivision( i , num_divisions );
                        
                        BlockList stateBlockList("stateBlockList");
                        state->getStateBlocksForDivision( i , & stateBlockList );
                        BlockInfo state_block_info = stateBlockList.getBlockInfo();
                        
                        BlockList *inputBlockList  = input->getInputBlockListForRange( range , operation_size );
                        BlockInfo input_block_info = inputBlockList->getBlockInfo();
                        
                        // Check latency of the input block
                        size_t latency = input_block_info.min_time_diff();
                        
                        // See if it is necessary to cancel ( not running the job ) for small size
                        
                        bool cancel_operation = false;
                        
                        if( input_block_info.size < operation_size )
                        {
                            //LM_M(("Caneled operation for size"));
                            cancel_operation = true;
                        }
                        
                        if( cancel_operation )
                        {
                            if( delayed_processing == "no" )   // No latency option
                                cancel_operation= false;
                            
                            if ( (max_latency > 0) && ( latency > max_latency) )
                            {
                                //LM_M(("Reduce state operation reopened for latency %lu max %lu" , latency , max_latency));
                                cancel_operation = false;
                            }
                        }
                        
                        // with no data, remove always
                        if( input_block_info.size  == 0 )
                            cancel_operation = true;
                        
                        // TODO: If size is not enougth, we can cancel operation here...
                        if( !cancel_operation )
                        {
                            //LM_M(("Planing an update operation for division %d" , i ));
                            //LM_M(("State blocks : %s" , stateBlockList->strRanges().c_str() ));
                            //LM_M(("Input blocks : %s" , inputBlockList->strRanges().c_str() ));
                            
                            size_t id = streamManager->queueTaskManager.getNewId();
                            
                            // Log activity in the worker
                            if( cmd.get_flag_string("stream_operation") != "no_stream_operation" )
                                streamManager->worker->logActivity( au::str("[ %s:%lu ] BB Update div %d/%d of %s %s with %s %s operation %s" , 
                                                                            cmd.get_flag_string("stream_operation").c_str() , 
                                                                            id,
                                                                            i , 
                                                                            num_divisions ,
                                                                            state_name.c_str(),
                                                                            stateBlockList.strShortDescription().c_str(),
                                                                            input_name.c_str(),    
                                                                            inputBlockList->strShortDescription().c_str(),
                                                                            operation_name.c_str()  ) );
                            
                            
                            
                            
                            network::StreamOperation *operation = getStreamOperation( op );
                            
                            ReduceQueueTask * task = new ReduceQueueTask( id , *operation , range );
                            task->addOutputsForOperation(op);
                            
                            // Spetial flag to indicate update_state mode ( process different output buffers )
                            task->setUpdateStateDivision( i );
                            
                            task->getBlockList("input_0")->copyFrom( inputBlockList );
                            task->getBlockList("input_1")->copyFrom( &stateBlockList );
                            
                            // remove input from the original queue
                            input->list->remove( inputBlockList );
                            
                            // Set the working size to get statictics at ProcessManager
                            task->setWorkingSize();
                            
                            // Add me as listener and increase the number of operations to run
                            task->addListenerId( getEngineId() );
                            num_pending_processes++;
                            
                            // Schedule tmp task into QueueTaskManager
                            streamManager->queueTaskManager.add( task );
                            
                            delete operation;
                        }
                        else
                        {
                            // Unlock state since it has not been used un any operation
                            state->unlockDivision( i );
                        }
                        
                        
                        // Remove list used here
                        if( inputBlockList)
                            delete inputBlockList;
                        
                    }
                }
                
                if( num_pending_processes == 0 )
                    finishWorkerTask();
                
                return;
            }
            
            if( cmd.get_argument(0) == "run_stream_operation" )
            {
                
                // Flag used in automatic update operation to lock input blocks and remove after operation
                bool clear_inputs =  cmd.get_flag_bool("clear_inputs"); 
                
                size_t min_size = cmd.get_flag_uint64("min_size");          // Minimum size to run an operation
                size_t max_latency = cmd.get_flag_uint64("max_latency");    // Max acceptable time to run an operation
                std::string delayed_processing = cmd.get_flag_string("delayed_processing");
                
                // Operation size    
                size_t default_size = SamsonSetup::shared()->getUInt64("general.memory") / SamsonSetup::shared()->getUInt64("general.num_processess");
                size_t operation_size = (min_size!=0)?min_size:default_size;
                
                BlockIdList block_ids;      // Collection of block ids for the already processed blocks ( only used in clear_inputs is not activated )
                
                //LM_M(("Worker command %s --> operation_size %lu" , command.c_str() , operation_size ));
                
                if( cmd.get_num_arguments() < 2 )
                {
                    finishWorkerTaskWithError( au::str("Not enougth parameters for command %s" , main_command.c_str() ) ); 
                    return;
                }
                
                std::string operation_name = cmd.get_argument(1);
                
                Operation *op = ModulesManager::shared()->getOperation(  operation_name );
                if( !op )
                {
                    finishWorkerTaskWithError( au::str("Unknown operation %s" , operation_name.c_str() ) ); 
                    return;
                }
                
                // Check input output arguments...
                if ( cmd.get_num_arguments() <  ( 2 + ( op->getNumInputs() + op->getNumOutputs() ) ) )
                {
                    finishWorkerTaskWithError( au::str("Not enougth arguments for operation %s" , operation_name.c_str() ) ); 
                    return;
                }

                // Check formats
                int pos_argument = 2; 
                for (int i = 0 ; i < op->getNumInputs() ; i++ )
                {
                    KVFormat operation_format = op->getInputFormat(i);
                    std::string queue_name = cmd.get_argument(pos_argument++);
                    KVFormat queue_format = streamManager->getQueue( queue_name )->format;
                    
                    if( queue_format != KVFormat("*","*") )
                        if( queue_format != operation_format )
                        {
                            finishWorkerTaskWithError( au::str("Format for input %d ( operation %s ) is %s while queue %s has format %s" 
                                                               , i , operation_name.c_str() , operation_format.str().c_str() , queue_name.c_str() , queue_format.str().c_str() ) );
                            return;
                        }
                    
                }
                
                /*
                 // NOTE: Outputs formats are not cheked any more since they can contain multiple outputs...
                for (int i = 0 ; i < op->getNumOutputs() ; i++ )
                {
                    KVFormat operation_format = op->getOutputFormat(i);
                    std::string queue_name = cmd.get_argument(pos_argument++);
                    KVFormat queue_format = streamManager->getQueue( queue_name )->format;
                    
                    if( queue_format != KVFormat("*","*") )
                        if( queue_format != operation_format )
                        {
                            finishWorkerTaskWithError( au::str("Format for output %d ( operation %s ) is %s while queue %s has format %s" 
                                                               , i , operation_name.c_str() , operation_format.str().c_str() , queue_name.c_str() , queue_format.str().c_str() ) );
                            return;
                        }
                }
                 */
                
                switch (op->getType()) {
                        
                    case Operation::reduce:
                    {
                        network::StreamOperation *operation = getStreamOperation( op );
                        
                        // Get the input queues
                        std::vector< Queue* > queues;
                        BlockInfo block_info;
                        int num_divisions = 1;
                        
                        
                        for (int i = 0 ; i < op->getNumInputs() ; i++ )
                        {
                            std::string input_queue_name = operation->input_queues(i);
                            Queue*queue = streamManager->getQueue( input_queue_name );
                            queue->update( block_info );
                            queues.push_back( queue );
                            
                            if( queue->num_divisions > num_divisions )
                                num_divisions = queue->num_divisions;
                        }
                        
                        // For each range, create a set of reduce operations...
                        for ( int r = 0 ; r < num_divisions ; r++ )
                        {
                            KVRange  range = rangeForDivision( r , num_divisions );
                            
                            
                            //Create the reduce operation
                            ReduceQueueTask *tmp = new ReduceQueueTask( streamManager->queueTaskManager.getNewId() , *operation , range ); 
                            tmp->addOutputsForOperation(op);
                            
                            // Take data from each input
                            for (int q = 0 ; q < (int) queues.size() ; q++ )
                            {
                                BlockList* list = tmp->getBlockList( au::str("input_%d" , q ) ); 
                                list->copyFrom( queues[q]->list , range );
                            }
                            
                            // Set the working size to get statictics at ProcessManager
                            tmp->setWorkingSize();
                            
                            // Add me as listener and increase the number of operations to run
                            tmp->addListenerId( getEngineId() );
                            num_pending_processes++;
                            
                            // Schedule tmp task into QueueTaskManager
                            streamManager->queueTaskManager.add( tmp );
                            
                        }
                        
                        
                        delete operation;
                        if( num_pending_processes == 0 )
                            finishWorkerTask();
                        
                        return;
                    }
                        break;             
                        
                    case Operation::parser:
                    case Operation::parserOut:
                    case Operation::map:
                    {
                        //LM_M(("Worker command %s min size %lu" , command.c_str() , min_size ));

                        network::StreamOperation *operation = getStreamOperation( op );
                        
                        // Get the input queue
                        std::string input_queue_name = operation->input_queues(0);
                        Queue *queue = streamManager->getQueue( input_queue_name );
                        
                        bool cancel_operation = false;
                        bool no_more_content = false;
                        
                        while( !cancel_operation && !no_more_content )
                        {
                            
                            // Get a BlockList with cotent to be processed
                            BlockList *inputData;
                            if( clear_inputs )
                                inputData = queue->getInputBlockListForProcessing( operation_size );
                            else
                                inputData = queue->getInputBlockListForProcessing( operation_size , &block_ids );
                            
                            if ( inputData->isEmpty() )
                                no_more_content = true; // No more data to be processed
                            
                            // Decide if operation has to be canceled
                            if( !no_more_content )
                            {
                                BlockInfo operation_block_info = inputData->getBlockInfo();
                                
                                // If there is not enougth size, do not run the operation
                                if( (min_size>0) && ( operation_block_info.size < min_size ) )
                                {
                                    cancel_operation = true;
                                }
                                
                                if( cancel_operation )
                                {
                                    if( delayed_processing == "no" )
                                        cancel_operation = false;
                                    
                                    // Check if latency is too high....
                                    if( ( max_latency > 0 ) && ( (size_t)operation_block_info.min_time_diff() > max_latency ) )
                                    {
                                        cancel_operation = false;       // No cancel since there is too much latency
                                    }
                                }
                            }
                            
                            if( !no_more_content && !cancel_operation )                            
                            {
                                // Get a new id for the next opertion
                                size_t id = streamManager->queueTaskManager.getNewId();
                                
                                QueueTask *tmp = NULL;
                                switch ( op->getType() ) {
                                    case Operation::parser:
                                    {
                                        tmp = new ParserQueueTask( id , *operation ); 
                                    }
                                        break;
                                        
                                    case Operation::map:
                                    {
                                        tmp = new MapQueueTask( id , *operation , KVRange(0,KVFILE_NUM_HASHGROUPS) ); 
                                    }
                                        break;
                                        
                                    case Operation::parserOut:
                                    {
                                        tmp = new ParserOutQueueTask( id , *operation , KVRange(0,KVFILE_NUM_HASHGROUPS) ); 
                                    }
                                        break;
                                        
                                    default:
                                        LM_X(1,("Internal error"));
                                        break;
                                }
                                
                                // Set the outputs    
                                tmp->addOutputsForOperation(op);
                                
                                // Copy input data
                                tmp->getBlockList("input_0")->copyFrom( inputData );
                                
                                // Set working size for correct monitorization of data
                                tmp->setWorkingSize();
                                
                                if( clear_inputs )
                                    tmp->environment.set("system.clear_imputs","yes");
                                
                                
                                // Add me as listener and increase the number of operations to run
                                tmp->addListenerId( getEngineId() );
                                num_pending_processes++;
                                
                                // Schedule tmp task into QueueTaskManager
                                streamManager->queueTaskManager.add( tmp );
                                
                                
                                if( cmd.get_flag_string("stream_operation") != "no_stream_operation" )
                                    streamManager->worker->logActivity( au::str("[ %s:%lu ] Processing %s from queue %s using operation %s" , 
                                                                                cmd.get_flag_string("stream_operation").c_str() , 
                                                                                id,
                                                                                inputData->strShortDescription().c_str(),
                                                                                input_queue_name.c_str() , 
                                                                                operation_name.c_str()  
                                                                       ));

                            }
                            else
                                queue->unlock( inputData );
                            
                            delete inputData;
                            
                        }
                        
                        delete operation;
                        if( num_pending_processes == 0 )
                            finishWorkerTask();
                        
                        return;
                    }
                        break;
                        
                        
                    default:
                    {
                        finishWorkerTaskWithError( au::str("run_stream_operation OK but it is still not implemented"  ) ); 
                    }
                        break;
                }
                
                
            }
            
            finishWorkerTaskWithError( au::str("Unknown command %s" , main_command.c_str()  ) ); 
            
        }
        
        void WorkerCommand::finishWorkerTaskWithError( std::string error_message )
        {
            error.set( error_message );
            finishWorkerTask();
        }
        
        void WorkerCommand::finishWorkerTask()
        {
            if( finished )
            {
                LM_W(("Trying to send a finish message for an already finished workerTask"));
                return;
            }
            
            if ( notify_finish )
            {
                Packet*           p = new Packet( Message::WorkerCommandResponse );
                network::WorkerCommandResponse* c = p->message->mutable_worker_command_response();
                c->mutable_worker_command()->CopyFrom( *originalWorkerCommand );
                
                // Put the error if any
                if( error.isActivated() )
                    c->mutable_error()->set_message( error.getMessage() );
                
                // Set delilah id
                p->message->set_delilah_id( delilah_id );
                
                // Send the packet
                streamManager->worker->network->send( fromId , p );
            }
            
            // Set the finished flag
            finished = true;
            
        }
        
        network::StreamOperation *WorkerCommand::getStreamOperation( Operation *op )
        {
            
            int pos_argument = 1;   // We skipt the "run_stream_operation" or "run_stream_update_state" parameter
            
            // Create the StreamOperation elements
            network::StreamOperation *operation  = new network::StreamOperation();
            operation->set_name("Manual operation");
            operation->set_operation( cmd.get_argument(pos_argument++) );
            
            
            for (int i = 0 ; i < op->getNumInputs() ; i++)
                operation->add_input_queues( cmd.get_argument( pos_argument++ ) );
            for (int i = 0 ; i < op->getNumOutputs() ; i++)
                operation->add_output_queues( cmd.get_argument( pos_argument++ ) );
            operation->set_num_workers( streamManager->worker->network->getNumWorkers() ); 
            
            return operation;
            
        }
        
        void WorkerCommand::notify( engine::Notification* notification )
        {
            if( notification->isName( notification_process_request_response ) )
            {
                num_pending_processes--;
                
                if( notification->environment.isSet("error") )
                    error.set( notification->environment.get("error" , "no_error") );
                
                checkFinish();
                return;
            }
            
            LM_W(("Unexpected notification at WorkerCommand"));
            
        }
        
        void WorkerCommand::checkFinish()
        {
            if( error.isActivated() )
                finishWorkerTask();
            else if( num_pending_processes <= 0 )
                finishWorkerTask();
        }

        void WorkerCommand::getInfo( std::ostringstream& output)
        {
            au::xml_open(output ,  "worker_command" );
            
            au::xml_simple( output , "command" , command );
            
            au::xml_close(output ,  "worker_command" );
        }
        
        
    }
}
