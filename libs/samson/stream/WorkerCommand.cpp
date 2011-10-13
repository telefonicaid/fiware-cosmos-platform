
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
            
            // Original value for the falg
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
            

            if( main_command == "add_stream_operation" )
            {
                if( cmd.get_num_arguments() < 3 )
                {
                    finishWorkerTaskWithError( "Usage: add_stream_operation name operation input1 input2 ... outputs1 outputs2 output3");
                    return;
                }
                
                std::string name            = cmd.get_argument( 1 );
                std::string operation       = cmd.get_argument( 2 );
                

                // If the operation exist, it will be replaced by this one, so no check if the stream operation is here

                // Check operation
                Operation* op = ModulesManager::shared()->getOperation(operation);
                if( !op )
                {
                    finishWorkerTaskWithError( "Unsupported operation " + operation );
                    return;
                }
                
                // Check the number of input / outputs
                if( cmd.get_num_arguments() < ( 3 + op->getNumInputs() +  op->getNumOutputs() ) )
                {
                    finishWorkerTaskWithError( au::str("Not enougth parameters for operation %s. It has %d inputs and %d outputs" , operation.c_str() , (int) op->getNumInputs() ,  (int) op->getNumOutputs() ) );
                    return;
                    
                }
                
                
                switch ( op->getType() ) {
                        
                    case Operation::parser:
                    case Operation::map:
                    case Operation::parserOut:
                        
                        // No additional checks
                        break;
                        
                        
                    case Operation::reduce:
                    {
                        if( op->getNumInputs() != 2 )
                        {
                            finishWorkerTaskWithError( au::str("Only reduce operations with 2 inputs are supported at the moment. ( In the furure, reducers with 3 or more inputs will be supported.") );
                            return;
                        }
                        
                        
                        // Check state format is coherent
                        KVFormat a = op->getInputFormats()[op->getNumInputs() - 1];
                        KVFormat b = op->getOutputFormats()[ op->getNumOutputs() - 1 ];
                        
                        if( !a.isEqual(b)  )
                        {
                            finishWorkerTaskWithError("Last input and output should be the same data type to qualify as stream-reduce");
                            return;                        
                        }
                        
                        // Check that the last input and the last output are indeed the same queue
                        std::string last_input = cmd.get_argument( 3 + op->getNumInputs() - 1 );
                        std::string last_output = cmd.get_argument( 3 + op->getNumInputs() + op->getNumOutputs() -1 );
                        if(  last_input !=  last_output )
                        {
                            finishWorkerTaskWithError( au::str("Last input and last output should be the same state. ( %s != %s)" , last_input.c_str() , last_output.c_str() ) ); 
                            return;
                            
                        }
                        
                    }
                        break;
                        
                    case Operation::script:
                    {
                        finishWorkerTaskWithError( "Script operations cannot be used to process stream queues. Only parsers, maps and spetial reducers" );
                        return;
                    }
                        
                        break;
                        
                    default:
                    {
                        finishWorkerTaskWithError( "Operation type is currently not supported... coming soon!" );
                        return;
                        
                    }
                        break;
                }
                
                
                // Create the new StreamOperation
                
                StreamOperation *stream_operation = new StreamOperation( streamManager );
                stream_operation->name = name;
                stream_operation->operation = operation;
                                              
                stream_operation->setNumWorkers( streamManager->worker->network->getNumWorkers() );
                
                int num_inputs  = op->getNumInputs();
                int num_outputs = op->getNumOutputs();
                
                for (int i = 0 ; i < num_inputs ; i++ )
                {
                    std::string queue_name = cmd.get_argument( 3 + i );
                    stream_operation->input_queues.push_back( queue_name );
                }
                
                for (int i = 0 ; i < num_outputs ; i++ )
                {
                    std::string queue_name = cmd.get_argument( 3 + num_inputs + i );
                    stream_operation->output_queues.push_back( queue_name );
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
                StreamOperation * operation = streamManager->stream_operations.findInMap( name );
                
                // Check if queue exist
                if( !operation  )
                {
                    finishWorkerTaskWithError( au::str("StreamOperation %s does not exist" , name.c_str()  ) );
                    return;
                }
                
                operation->setActive( false );
                finishWorkerTask();
                return;
                
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
                
                // Set the environemnt variable
                operation->environment.set( property , value );
                
                finishWorkerTask();
                return;
                

            }            
            
            if( cmd.get_argument(0) == "review_stream_operations" )
            {
                while( true ) 
                {

                    // Only run stream operations if there is space in the processing engine...
                    if( !engine::ProcessManager::shared()->hasFreeCores() )
                    {
                        if( finished )
                            return;
                        if( num_pending_processes == 0 )
                            finishWorkerTask();
                        return; // Nothing more to review
                    }
                    
                    // Decide the next StreamOperation 
                    StreamOperation* stream_operation = NULL;
                    au::map <std::string , StreamOperation>::iterator it_stream_operations;
                    for( it_stream_operations = streamManager->stream_operations.begin() ; it_stream_operations != streamManager->stream_operations.end() ; it_stream_operations++ )
                    {
                        StreamOperation* _stream_operation = it_stream_operations->second;

                        if( _stream_operation->ready( ) ) // Check if stream operation needs to be fired
                        {
                            // Check if this has higher priority
                            if(  _stream_operation->compare( stream_operation ) )
                                stream_operation = _stream_operation;
                        }
                            
                    }

                    LM_M(("Lopp review_stream_operations --> %s" , stream_operation->name.c_str()));
                    
                    if( stream_operation )
                        review_stream_operation( stream_operation );
                    else
                    {
                        if( finished )
                            return;
                        if( num_pending_processes == 0 )
                            finishWorkerTask();
                        return; // Nothing more to review
                    }
                    
                    
                }
 
                /*
                au::map <std::string , StreamOperation>::iterator it_stream_operations;
                for( it_stream_operations = streamManager->stream_operations.begin() ; it_stream_operations != streamManager->stream_operations.end() ; it_stream_operations++ )
                {
                    std::string stream_operation_name = it_stream_operations->first;
                    review_stream_operation( stream_operation_name );
                }
                 */

            }
            
            // Manually driven stream operations..... ( batch operations over stream queues )
            
            if( cmd.get_argument(0) == "run_stream_operation" )
            {
                
                // Flag used in automatic update operation to lock input blocks and remove after operation
                bool clear_inputs =  cmd.get_flag_bool("clear_inputs"); 
                
                size_t min_size = SamsonSetup::shared()->getUInt64("stream.min_operation_input_size");          // Minimum size to run an operation
                size_t max_size = SamsonSetup::shared()->getUInt64("stream.max_operation_input_size");          // Minimum size to run an operation
                
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
                        StreamOperation *operation = getStreamOperation( op );
                        
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
                            
                            if( queue->num_divisions > num_divisions )
                                num_divisions = queue->num_divisions;
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
                        
                        StreamOperation *operation = getStreamOperation( op );
                        
                        // Get the input queue
                        std::string input_queue_name = operation->input_queues[0];
                        Queue *queue = streamManager->getQueue( input_queue_name );
                        
                        bool cancel_operation = false;
                        bool no_more_content = false;
                        
                        while( !cancel_operation && !no_more_content )
                        {
                            
                            // Get a BlockList with cotent to be processed
                            BlockList *inputData;
                            if( clear_inputs )
                                inputData = queue->getInputBlockListForProcessing( max_size );
                            else
                                inputData = queue->getInputBlockListForProcessing( max_size , &block_ids );
                            
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
                            }
                            
                            if( !no_more_content && !cancel_operation )                            
                            {
                                // Get a new id for the next opertion
                                size_t id = streamManager->queueTaskManager.getNewId();
                                
                                QueueTask *tmp = NULL;
                                switch ( op->getType() ) {
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
                    c->mutable_error()->set_message( error.getMessage() );
                
                // Set delilah id
                p->message->set_delilah_id( delilah_id );
                
                // Send the packet
                streamManager->worker->network->send( fromId , p );
            }
            
            // Set the finished flag
            finished = true;
            
        }
        
        StreamOperation *WorkerCommand::getStreamOperation( Operation *op )
        {
            
            int pos_argument = 1;   // We skipt the "run_stream_operation" or "run_stream_update_state" parameter
            
            // Create the StreamOperation elements
            StreamOperation *operation  = new StreamOperation(streamManager);
            operation->name = "Manual operation";
            operation->operation = cmd.get_argument(pos_argument++);
            
            
            for (int i = 0 ; i < op->getNumInputs() ; i++)
                operation->input_queues.push_back( cmd.get_argument( pos_argument++ ) );
            for (int i = 0 ; i < op->getNumOutputs() ; i++)
                operation->output_queues.push_back( cmd.get_argument( pos_argument++ ) );
            
            operation->setNumWorkers(  streamManager->worker->network->getNumWorkers() ); 
            
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
        
        
        void WorkerCommand::review_stream_operation( StreamOperation* stream_operation )
        {
            
            // Get the operation itself
            Operation *op = ModulesManager::shared()->getOperation(  stream_operation->operation );
            if( !op )
            {
                finishWorkerTaskWithError( au::str("Unknown operation %s" , stream_operation->operation.c_str() ) ); 
                return;
            }
            
            switch (op->getType()) 
            {
                    
                    // Forward stream operations
                case Operation::parser:             
                case Operation::parserOut:
                case Operation::map:
                {
                    // Run forward kind of stream operations
                    review_stream_operation_forward( stream_operation);
                    return;
                }
                    
                case Operation::reduce:
                {
                    // Run forward kind of stream operations
                    review_stream_operation_reduce( stream_operation);
                    return;
                }
                    
                    
                default:
                    LM_X(1,("Internal error"));
                    break;
            }            
        }
        
        
        void WorkerCommand::review_stream_operation_forward(  StreamOperation* stream_operation )
        {
            
            // Get the operation itself
            Operation *op = ModulesManager::shared()->getOperation(  stream_operation->operation );
            
            // Properties for this stream operation
            size_t max_size         = SamsonSetup::shared()->getUInt64("stream.max_operation_input_size");          // Minimum size to run an operation
            
            // Get the input queue
            std::string input_queue_name = stream_operation->input_queues[0];
            Queue *queue = streamManager->getQueue( input_queue_name );
            
            // Get a BlockList with cotent to be processed
            BlockList *inputData = queue->getInputBlockListForProcessing( max_size );
            
            
            // Get a new id for the next opertion
            size_t id = streamManager->queueTaskManager.getNewId();
            
            QueueTask *tmp = NULL;
            switch ( op->getType() ) {
                case Operation::parser:
                {
                    tmp = new ParserQueueTask( id , stream_operation ); 
                }
                    break;
                    
                case Operation::map:
                {
                    tmp = new MapQueueTask( id , stream_operation , KVRange(0,KVFILE_NUM_HASHGROUPS) ); 
                }
                    break;
                    
                case Operation::parserOut:
                {
                    tmp = new ParserOutQueueTask( id , stream_operation , KVRange(0,KVFILE_NUM_HASHGROUPS) ); 
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
            
            // Update information about this operation
            stream_operation->add( tmp );
            
            // Set this property to cleat inputs at the end of the operation
            tmp->environment.set("system.clear_imputs","yes");
            
            // Add me as listener and increase the number of operations to run
            tmp->addListenerId( getEngineId() );
            num_pending_processes++;
            
            // Schedule tmp task into QueueTaskManager
            streamManager->queueTaskManager.add( tmp );
            
            // Log activity    
            streamManager->worker->logActivity( au::str("[ %s:%lu ] Processing %s from queue %s using operation %s" , 
                                                        stream_operation->name.c_str() , 
                                                        id,
                                                        inputData->strShortDescription().c_str(),
                                                        input_queue_name.c_str() , 
                                                        stream_operation->name.c_str()  
                                                        ));
            
            
            
        }
        
        void WorkerCommand::review_stream_operation_reduce( StreamOperation* stream_operation )
        {
            // Get the operation itself
            Operation *op = ModulesManager::shared()->getOperation(  stream_operation->operation );
            
            // Properties for this stream operation
            bool block_break_mode = ( stream_operation->environment.get("block_break_mode", "yes") == "yes" );
            
            std::string input_name = stream_operation->input_queues[0];
            std::string state_name = stream_operation->input_queues[1];
            
            Queue *state = streamManager->getQueue( state_name );
            Queue *input = streamManager->getQueue( input_name );
            
            // Check what operations can be done....
            int num_divisions = state->num_divisions;      // Divisions used in update-state mode
            
            
            if( !block_break_mode )
            {
                // NON - BLOCK BREAK MODE -
                
                // In this case, reduce-update state operations are schedules only if all divisions are ready
                // First we select all blocks to be processed ( max memory_size/2 )
                
                BlockInfo input_block_info = input->list->getBlockInfo();
                
                if( !state->lockAllDivisions() )
                {
                    LM_X(1,("Internal error. If we are here is because stream_operation is ready"));
                }

                // Update information about number of update-states
                stream_operation->add_update_state();
                
                // Increase the counter for this queue
                state->updates++;
                
                // Select all possible blocks
                size_t max_common_size = 0.75*(double)SamsonSetup::shared()->getUInt64("general.memory");
                
                BlockList global_inputBlockList;
                global_inputBlockList.extractFrom(input->list, max_common_size );
                
                // remove input from the original queue
                input->list->remove( &global_inputBlockList );       // Remove input all blocks ( not necessary
                
                // Log activity in the worker
                streamManager->worker->logActivity( au::str("[ %s ] Global update %s %s (%d divs) with %s %s operation %s" , 
                                                            stream_operation->name.c_str() , 
                                                            state_name.c_str(),
                                                            state->list->strShortDescription().c_str(),
                                                            num_divisions,
                                                            input_name.c_str(),    
                                                            global_inputBlockList.strShortDescription().c_str(),
                                                            stream_operation->name.c_str()  ) );
                
                
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
                        streamManager->worker->logActivity( au::str("[ %s:%lu ] Update div %d/%d of %s %s with %s %s operation %s" , 
                                                                    stream_operation->name.c_str() , 
                                                                    id,
                                                                    i , 
                                                                    num_divisions ,
                                                                    state_name.c_str(),
                                                                    stateBlockList.strShortDescription().c_str(),
                                                                    input_name.c_str(),    
                                                                    inputBlockList.strShortDescription().c_str(),
                                                                    stream_operation->name.c_str()  ) );
                        
                        
                        
                        
                        
                        ReduceQueueTask * task = new ReduceQueueTask( id , stream_operation , range );
                        task->addOutputsForOperation(op);
                        
                        // Spetial flag to indicate update_state mode ( process different output buffers )
                        task->setUpdateStateDivision( i );
                        
                        task->getBlockList("input_0")->copyFrom( &inputBlockList );
                        task->getBlockList("input_1")->copyFrom( &stateBlockList );
                        
                        // Set the working size to get statictics at ProcessManager
                        task->setWorkingSize();
                        
                        
                        // Update information about this operation
                        stream_operation->add( task );
                        
                        // Add me as listener and increase the number of operations to run
                        task->addListenerId( getEngineId() );
                        num_pending_processes++;
                        
                        // Schedule tmp task into QueueTaskManager
                        streamManager->queueTaskManager.add( task );
                        
                        
                    }
                    else
                    {
                        // There are no operations for this divisions
                        state->unlockDivision( i );
                    }
                    
                }
                
            }
            else
            {
                // BLOCK BREAK MODE
                
                size_t max_size         = SamsonSetup::shared()->getUInt64("stream.max_operation_input_size");          // Minimum size to run an operation
                
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
                    
                    BlockList *inputBlockList  = input->getInputBlockListForRange( range , max_size );
                    BlockInfo input_block_info = inputBlockList->getBlockInfo();
                    
                    // See if it is necessary to cancel ( not running the job ) for small size
                    if( input_block_info.size > 0 )
                    {
                        //LM_M(("Planing an update operation for division %d" , i ));
                        //LM_M(("State blocks : %s" , stateBlockList->strRanges().c_str() ));
                        //LM_M(("Input blocks : %s" , inputBlockList->strRanges().c_str() ));
                        
                        size_t id = streamManager->queueTaskManager.getNewId();
                        
                        // Log activity in the worker
                        streamManager->worker->logActivity( au::str("[ %s:%lu ] BB Update div %d/%d of %s %s with %s %s operation %s" , 
                                                                    stream_operation->name.c_str() , 
                                                                    id,
                                                                    i , 
                                                                    num_divisions ,
                                                                    state_name.c_str(),
                                                                    stateBlockList.strShortDescription().c_str(),
                                                                    input_name.c_str(),    
                                                                    inputBlockList->strShortDescription().c_str(),
                                                                    stream_operation->name.c_str()  ) );
                        
                        
                        
                        
                        
                        ReduceQueueTask * task = new ReduceQueueTask( id , stream_operation , range );
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
            
            return;
        }            
        
        
    }
}
