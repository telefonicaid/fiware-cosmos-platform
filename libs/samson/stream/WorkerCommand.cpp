
#include <algorithm>

#include "au/ErrorManager.h"

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
            
            // Extract environment properties
            copyEnviroment( originalWorkerCommand->environment() , &enviroment );
            
            // Extract command for simplicity
            command = originalWorkerCommand->command();
            
            // Parse command
            cmd.set_flag_boolean("clear_inputs");
            
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
            cmd.parse( command );
            
            // Original value for the flag
            pending_to_be_executed =  true;
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
            
            // Not pending any more, except of something happen...
            pending_to_be_executed = false;
            
            // Main command
            std::string main_command = cmd.get_argument(0);
            
            
            if( main_command == "wait" )
            {
                // Special operation to wait until no activity is present in stream manager
                
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
                    finishWorkerTaskWithError( au::str("Not enough parameters for command %s" , main_command.c_str() ) );
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
                    finishWorkerTaskWithError( au::str("Not enough parameters for command %s" , main_command.c_str() ) );
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
                    finishWorkerTaskWithError( au::str("Not enough parameters for command %s" , main_command.c_str() ) );
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
            
            

            if( main_command == "add_stream_operation" )
            {

                au::ErrorManager tmp_error;
                StreamOperation* stream_operation = StreamOperation::newStreamOperation(streamManager,  command , tmp_error );

                if( !stream_operation )
                {
                    finishWorkerTaskWithError( tmp_error.getMessage() );
                    return;
                }
                
                // Add this new stream operation
                streamManager->add( stream_operation );

                finishWorkerTask();
                return;
            }
            
            if( main_command == "rm_stream_operation" )
            {
                // Remove a particular stream operation
                
                if( cmd.get_num_arguments() < 2 )
                {
                    finishWorkerTaskWithError( "Usage: rm_stream_operation name " );
                    return;
                }
                
                std::string name            = cmd.get_argument( 1 );
                
                // Check it the queue already exists
                StreamOperation * operation = streamManager->stream_operations.extractFromMap( name );
                
                // Check if queue exist
                if( !operation  )
                {
                    finishWorkerTaskWithError( au::str("StreamOperation %s does not exist" , name.c_str()  ) );
                    return;
                }
                else
                {
                    delete operation;
                    finishWorkerTask();
                    return;
                }
                
            }
            
            if( main_command == "set_stream_operation_property" )
            {
                if( cmd.get_num_arguments() < 4 )
                {
                    finishWorkerTaskWithError( "Usage: set_stream_operation_property name property value " );
                    return;
                }
                
                std::string name            = cmd.get_argument( 1 );
                std::string property        = cmd.get_argument( 2 );
                std::string value           = cmd.get_argument( 3 );
                
                // Check it the queue already exists
                StreamOperation * operation = streamManager->stream_operations.findInMap( name );
                
                // Check if queue exist
                if( !operation  )
                {
                    finishWorkerTaskWithError( au::str("StreamOperation %s does not exist" , name.c_str()  ) );
                    return;
                }
                
                // Set the environment variable
                operation->environment.set( property , value );
                
                finishWorkerTask();
                return;
                

            }            
            
            if( main_command == "connect_to_queue" )
            {
                if( cmd.get_num_arguments() < 2 )
                {
                    finishWorkerTaskWithError( "Usage: connect_to_queue queue" );
                    return;
                }
                std::string queue = cmd.get_argument(1);
                
                streamManager->connect_to_queue( fromId , queue );
                
                finishWorkerTask();
                return;
                
            }

            if( main_command == "disconnect_from_queue" )
            {
                if( cmd.get_num_arguments() < 2 )
                {
                    finishWorkerTaskWithError( "Usage: connect_to_queue queue" );
                    return;
                }
                std::string queue = cmd.get_argument(1);
                
                streamManager->disconnect_from_queue( fromId , queue );
                
                finishWorkerTask();
                return;
                
            }
            
            
            // Manually driven stream operations..... ( batch operations over stream queues )
            
            if( cmd.get_argument(0) == "run_stream_operation" )
            {
                
                // Flag used in automatic update operation to lock input blocks and remove after operation
                bool clear_inputs =  cmd.get_flag_bool("clear_inputs"); 
                
                //size_t min_size = SamsonSetup::shared()->getUInt64("stream.min_operation_input_size");          // Minimum size to run an operation
                size_t max_size = SamsonSetup::shared()->getUInt64("stream.max_operation_input_size");          // Minimum size to run an operation
                
                BlockIdList block_ids;      // Collection of block ids for the already processed blocks ( only used in clear_inputs is not activated )
                
                //LM_M(("Worker command %s --> operation_size %lu" , command.c_str() , operation_size ));
                
                if( cmd.get_num_arguments() < 2 )
                {
                    finishWorkerTaskWithError( au::str("Not enough parameters for command %s" , main_command.c_str() ) );
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
                    finishWorkerTaskWithError( au::str("Not enough arguments for operation %s" , operation_name.c_str() ) );
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
                        StreamOperationBase *operation = getStreamOperation( op );
                        
                        // Get the input queues
                        std::vector< Queue* > queues;
                        BlockInfo block_info;
                        int num_divisions = 1;
                        
                        
                        for (int i = 0 ; i < op->getNumInputs() ; i++ )
                        {
                            std::string input_queue_name = operation->input_queues[i];
                            Queue*queue = streamManager->getQueue( input_queue_name );
                            queue->update( block_info );
                            queues.push_back( queue );                            
                        }
                        
                        // For each range, create a set of reduce operations...
                        for ( int r = 0 ; r < num_divisions ; r++ )
                        {
                            KVRange  range = rangeForDivision( r , num_divisions );
                            
                            
                            //Create the reduce operation
                            ReduceQueueTask *tmp = new ReduceQueueTask( streamManager->queueTaskManager.getNewId() , operation , range ); 
                            tmp->addOutputsForOperation(op);
                            
                            // Take data from each input
                            for (int q = 0 ; q < (int) queues.size() ; q++ )
                            {
                                BlockList* list = tmp->getBlockList( au::str("input_%d" , q ) ); 
                                list->copyFrom( queues[q]->list , range );
                            }
                            
                            // Set the working size to get statistics at ProcessManager
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
                        
                        StreamOperationBase *operation = getStreamOperation( op );
                        
                        // Get the input queue
                        std::string input_queue_name = operation->input_queues[0];
                        Queue *queue = streamManager->getQueue( input_queue_name );
                        
                        
                        BlockList inputBlockList;
                        if( clear_inputs )
                            inputBlockList.extractFrom( queue->list , 0 );
                        else
                            inputBlockList.copyFrom( queue->list ,  0 );
                        
                        while( !inputBlockList.isEmpty() )
                        {
                            
                            // Get a BlockList with content to be processed
                            BlockList inputData("input data");
                            inputData.extractFrom( &inputBlockList , max_size );
                            
                            // Get a new id for the next operation
                            size_t id = streamManager->queueTaskManager.getNewId();
                            
                            QueueTask *tmp = NULL;
                            switch ( op->getType() ) 
                            {
                                case Operation::parser:
                                {
                                    tmp = new ParserQueueTask( id , operation ); 
                                }
                                    break;
                                    
                                case Operation::map:
                                {
                                    tmp = new MapQueueTask( id , operation , KVRange(0,KVFILE_NUM_HASHGROUPS) ); 
                                }
                                    break;
                                    
                                case Operation::parserOut:
                                {
                                    tmp = new ParserOutQueueTask( id , operation , KVRange(0,KVFILE_NUM_HASHGROUPS) ); 
                                }
                                    break;
                                    
                                default:
                                    LM_X(1,("Internal error"));
                                    break;
                            }
                            
                            // Set the outputs    
                            tmp->addOutputsForOperation(op);
                            
                            // Copy input data
                            tmp->getBlockList("input_0")->copyFrom( &inputData );
                            
                            // Set working size for correct monitoring of data
                            tmp->setWorkingSize();
                            
                            if( clear_inputs )
                                tmp->environment.set("system.clear_imputs","yes");
                            
                            
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
            //LM_M(("Setting error message %s" , error_message.c_str() ));
            error.set( error_message );
            finishWorkerTask();
        }
        
        void WorkerCommand::finishWorkerTask()
        {
            if( finished )
            {
                assert(false);
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
                {
                    //LM_M(("Sending error message %s" , error.getMessage().c_str() ));
                    c->mutable_error()->set_message( error.getMessage() );
                }
                
                // Set delilah id
                p->message->set_delilah_id( delilah_id );
                
                // Send the packet
                streamManager->worker->network->send( fromId , p );
            }
            
            // Set the finished flag
            finished = true;
            
        }
        
        StreamOperationBase *WorkerCommand::getStreamOperation( Operation *op )
        {
            
            int pos_argument = 1;   // We skip the "run_stream_operation" or "run_stream_update_state" parameter
            
            std::string operation_name = cmd.get_argument(pos_argument++);

            StreamOperationBase *operation  = new StreamOperationBase( operation_name , streamManager->worker->network->getNumWorkers()  );
            
            for (int i = 0 ; i < op->getNumInputs() ; i++)
                operation->input_queues.push_back( cmd.get_argument( pos_argument++ ) );
            for (int i = 0 ; i < op->getNumOutputs() ; i++)
                operation->output_queues.push_back( cmd.get_argument( pos_argument++ ) );
            
            
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
