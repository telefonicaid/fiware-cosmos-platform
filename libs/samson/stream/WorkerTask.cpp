

#include "engine/Notification.h"
#include "engine/ProcessManager.h"  


#include "samson/common/EnvironmentOperations.h"        // copyEnvironment

#include "samson/network/Packet.h"                      // samson::Packet

#include "samson/worker/SamsonWorker.h"

#include "samson/stream/StreamManager.h"                // samson::stream::StreamManager
#include "samson/stream/Queue.h"
#include "samson/stream/QueueItem.h"
#include "samson/stream/BlockList.h"

#include "samson/stream/ParserQueueTask.h"

#include "WorkerTask.h"     // Own interface

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
            
            // Original value for the falg
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
            
            // Main command
            std::string main_command = cmd.get_argument(0);
            
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
                    std::string queue_name = cmd.get_argument(1);
                    streamManager->remove_queue(  queue_name );
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
            
            
            if( cmd.get_argument(0) == "run_stream_operation" )
            {
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
                
                
                // Additional common flags in command
                bool clear_inputs =  cmd.get_flag_bool("clear_inputs");
                
                //size_t max_size_per_operation = 1000000000;
                size_t max_size_per_operation =  100000000; // 100Mb for testing
                
                switch (op->getType()) {
                        
                    case Operation::parser:
                    {
                        
                        network::StreamOperation *operation = getStreamOperation( op );
                        
                        // Get the input queue
                        std::string input_queue_name = operation->input_queues(0);
                        Queue *q = streamManager->getQueue( input_queue_name );
                        
                        // Iterate thougth all the queue-items
                        au::list< QueueItem >::iterator item;
                        for ( item = q->items.begin() ; item != q->items.end() ; item++ )
                        {
                            QueueItem *queueItem = *item;
                            
                            // Get a copy of all the blocks included in this queue
                            BlockList localBlockList("local_block_list_planning_operation");
                            localBlockList.copyFrom( queueItem->list );
                            
                            if( clear_inputs )
                                queueItem->list->clearBlockList();
                            
                            while( !localBlockList.isEmpty() )
                            {
                                // Create the map operation
                                size_t id = streamManager->queueTaskManager.getNewId();
                                ParserQueueTask *tmp = new ParserQueueTask( id , *operation ); 
                                tmp->addOutputsForOperation(op);
                                
                                // Extract the rigth blocks from queue
                                tmp->list->extractFrom( &localBlockList , max_size_per_operation );

                                // Add me as listener and increase the number of operations to run
                                tmp->addListenerId( getEngineId() );
                                num_pending_processes++;
                                
                                // Schedule tmp task into QueueTaskManager
                                streamManager->queueTaskManager.add( tmp );
                                
                            }
                        }
                        
                        delete operation;
                        return;
                    }
                        break;
                        
                    case Operation::parserOut:
                    {
                        
                        network::StreamOperation *operation = getStreamOperation( op );
                        
                        // Get the input queue
                        std::string input_queue_name = operation->input_queues(0);
                        Queue *q = streamManager->getQueue( input_queue_name );
                        
                        // Iterate thougth all the queue-items
                        au::list< QueueItem >::iterator item;
                        for ( item = q->items.begin() ; item != q->items.end() ; item++ )
                        {
                            QueueItem *queueItem = *item;
                            
                            // Get a copy of all the blocks included in this queue
                            BlockList localBlockList("local_block_list_planning_operation");
                            localBlockList.copyFrom( queueItem->list );
                            
                            if( clear_inputs )
                                queueItem->list->clearBlockList();
                            
                            while( !localBlockList.isEmpty() )
                            {
                                // Create the map operation
                                size_t id = streamManager->queueTaskManager.getNewId();
                                ParserOutQueueTask *tmp = new ParserOutQueueTask( id , *operation , (*item)->range ); 
                                tmp->addOutputsForOperation(op);
                                
                                // Extract the rigth blocks from queue
                                tmp->list->extractFrom( &localBlockList , max_size_per_operation );
                                
                                // Schedule tmp task into QueueTaskManager
                                streamManager->queueTaskManager.add( tmp );
                                
                            }
                        }
                        
                        delete operation;
                        return;
                    }
                        break;
                        
                    case Operation::map:
                    {
                        
                        network::StreamOperation *operation = getStreamOperation( op );
                        
                        // Get the input queue
                        std::string input_queue_name = operation->input_queues(0);
                        Queue *q = streamManager->getQueue( input_queue_name );
                        
                        // Iterate thougth all the queue-items
                        au::list< QueueItem >::iterator item;
                        for ( item = q->items.begin() ; item != q->items.end() ; item++ )
                        {
                            QueueItem *queueItem = *item;
                            
                            // Get a copy of all the blocks included in this queue
                            BlockList localBlockList("local_block_list_planning_operation");
                            localBlockList.copyFrom( queueItem->list );
                            
                            if( clear_inputs )
                                queueItem->list->clearBlockList();
                            
                            while( !localBlockList.isEmpty() )
                            {
                                // Create the map operation
                                MapQueueTask *tmp = new MapQueueTask( streamManager->queueTaskManager.getNewId() , *operation , (*item)->range ); 
                                tmp->addOutputsForOperation(op);
                                
                                // Extract the rigth blocks from queue
                                tmp->list->extractFrom( &localBlockList , max_size_per_operation );
                                
                                // Add me as listener and increase the number of operations to run
                                tmp->addListenerId( getEngineId() );
                                num_pending_processes++;
                                
                                // Schedule tmp task into QueueTaskManager
                                streamManager->queueTaskManager.add( tmp );
                            }
                        }
                        delete operation;
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
            
            int pos_argument = 1;   // We skipt the "run_stream_operation" parameter
            
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