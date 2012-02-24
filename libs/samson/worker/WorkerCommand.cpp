
#include <algorithm>

#include "au/utils.h"
#include "au/ErrorManager.h"
#include "au/string.h"

#include "engine/Notification.h"
#include "engine/ProcessManager.h"  


#include "samson/common/EnvironmentOperations.h"        // copyEnvironment
#include "samson/common/SamsonSetup.h"                  // SamsonSetup

#include "samson/network/Packet.h"                      // samson::Packet

#include "samson/worker/SamsonWorker.h"

#include "samson/stream/StreamManager.h"                // samson::stream::StreamManager
#include "samson/stream/Queue.h"
#include "samson/stream/BlockList.h"
#include "samson/stream/StreamOperation.h"

#include "samson/stream/QueueTasks.h"

#include "WorkerCommand.h"     // Own interface

namespace samson {
    
    bool ignoreCommand( std::string command )
    {
        if( command.length() == 0 )
            return true;
        
        for ( size_t i=0 ; i<command.length() ; i++)
            if ( command[i] != ' ' )
            {
                if ( command[i] == '#' )
                    return true;
                else
                    return false;
            }
        
        return true;
    }
    
    class AliasManager
    {
        au::simple_map<std::string, std::string> aliases;
        
    public:
        
        void add( std::string reference , std::string name )
        {
            aliases.insertInMap(reference, name);
        }
        
        std::string transform( std::string command )
        {
            au::simple_map<std::string, std::string>::iterator it_aliases;
            
            for ( it_aliases = aliases.begin() ; it_aliases != aliases.end() ; it_aliases++ )
                au::find_and_replace( command , it_aliases->first , it_aliases->second );
            
            return command;
        }
        
        
    };
    
    WorkerCommand::WorkerCommand( size_t _delilah_id , size_t _delilah_component_id ,  const network::WorkerCommand& _command )
    {
        samsonWorker = NULL;
        
        //Identifiers to notify when finished
        delilah_id = _delilah_id;
        delilah_component_id = _delilah_component_id;
        
        notify_finish = true;
        
        // Copy the original message
        originalWorkerCommand = new network::WorkerCommand();
        originalWorkerCommand->CopyFrom( _command );
        
        // Extract environment properties
        copyEnviroment( originalWorkerCommand->environment() , &enviroment );
        
        // Extract command for simplicity
        command = originalWorkerCommand->command();
        
        // Original value for the flags
        pending_to_be_executed =  true;
        finished = false;
        
        // No pending process at the moment
        num_pending_processes = 0;
    }
    
    WorkerCommand::WorkerCommand( std::string _command )
    {
        samsonWorker = NULL;
        
        // Not necessary to notify
        originalWorkerCommand = NULL;
        notify_finish = false;
        
        // Get directly the command to run
        command = _command;
        
        
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
    
    void WorkerCommand::setSamsonWorker( SamsonWorker * _samsonWorker )
    {
        samsonWorker = _samsonWorker;
        streamManager = samsonWorker->streamManager;
    }
    
    
    bool WorkerCommand::isFinished()
    {
        return finished;
    }
    
    void WorkerCommand::runCommand( std::string command , au::ErrorManager* error )
    {
        
        //LM_M(("WC Running command '%s'" , command.c_str() ));
        
        if( ignoreCommand( command ) )
            return;
        
        // Parse command
        au::CommandLine cmd;
        cmd.set_flag_boolean("clear_inputs");
        cmd.set_flag_boolean("f");
        
        cmd.set_flag_boolean("v");
        cmd.set_flag_boolean("vv");
        cmd.set_flag_boolean("vvv");
        
        cmd.set_flag_boolean("new");
        cmd.set_flag_boolean("remove");
        cmd.set_flag_string("prefix", "");
        cmd.parse( command );
        
        std::string prefix = cmd.get_flag_string("prefix");
        
        if ( cmd.get_num_arguments() == 0 )
        {
            error->set("No command provided");
            return;
        }
        
        std::string main_command = cmd.get_argument(0);

        if( main_command == "init_stream" )
        {
            if( cmd.get_num_arguments() < 2 )
            {
                error->set( au::str("Not enough parameters for command 'init_stream' ( only %d argument provided )" , cmd.get_num_arguments()  ) );
                return;
            }
            
            std::string operation_name;
            
            if( cmd.get_num_arguments() == 3 )
            {
                prefix.append( cmd.get_argument(1) );
                prefix.append("/");
                
                operation_name  = cmd.get_argument(2);
            }
            else if( cmd.get_num_arguments() == 2 )
            {
                operation_name  = cmd.get_argument(1);
            }
            
            Operation *op = ModulesManager::shared()->getOperation(  operation_name );
            if( !op )
            {
                error->set( au::str("Unknown operation %s" , operation_name.c_str() ) ); 
                return;
            }
            
            if (op->getType() != Operation::script )
            {
                error->set( 
                           au::str("Non valid operation %d. Only script operations supported for init_stream command" 
                                   , operation_name.c_str()  ) ); 
                return;
            }             
            
            
            // Structure to set names alias
            AliasManager alias_manager;
            
            // Read code, execute it recursivelly
            au::ErrorManager sub_error;
            for ( size_t i = 0 ; i < op->code.size() ; i++ )
            {
                std::string sub_command = op->code[i];
                
                
                au::CommandLine intern_cmdLine;
                intern_cmdLine.parse( sub_command );
                
                if( intern_cmdLine.get_argument(0) == "alias" )
                {
                    if( intern_cmdLine.get_num_arguments() < 3 )
                    {
                        error->set( au::str("Not enough parameters for command %s" , main_command.c_str() ) );
                        return;
                    }
                    
                    std::string reference = intern_cmdLine.get_argument(1);
                    std::string value = intern_cmdLine.get_argument(2);
                    
                    //LM_M(("Alias %s=%s", reference.c_str() , value.c_str() ));
                    
                    alias_manager.add( reference , value);
                }
                else
                {
                    std::string full_command = alias_manager.transform( sub_command );
                    
                    if( prefix.length() > 0 )
                        full_command.append( au::str(" -prefix %s" , prefix.c_str() ) );
                    
                    //LM_M(("Full Command %s (original %s)" , full_command.c_str(),  sub_command.c_str() ));
                    
                    runCommand( full_command , &sub_error);
                }
                
                if( sub_error.isActivated() )
                {
                    error->set( au::str("[%s:%d]%s" , operation_name.c_str() , i , sub_error.getMessage().c_str() ) );
                    return;
                }
                
            }
            
            return;
            
            
        }
        
        if( main_command == "remove_all_stream" )
        {
            samsonWorker->streamManager->reset();
            return;
        }
        
        if( main_command == "reload_modules" )
        {
            // Spetial operation to reload modules
            ModulesManager::shared()->reloadModules();
            return;
        }
        
        if ( cmd.get_argument(0) == "rm_queue" )
        {
            if( cmd.get_num_arguments() < 2 )
                error->set( au::str("Not enough parameters for command %s" , main_command.c_str() ) );
            else
            {
                for ( int i = 1 ; i < cmd.get_num_arguments() ; i++ )
                {
                    std::string queue_name = prefix + cmd.get_argument(i);
                    samsonWorker->streamManager->remove_queue(  queue_name );
                }
            }
            return;
        }
        
        if ( cmd.get_argument(0) == "cp_queue" )
        {
            if( cmd.get_num_arguments() < 3 )
                error->set( au::str("Not enough parameters for command %s" , main_command.c_str() ) );
            else
            {
                std::string from_queue_name = prefix + cmd.get_argument(1);
                std::string to_queue_name   = prefix + cmd.get_argument(2);
                samsonWorker->streamManager->cp_queue(  from_queue_name , to_queue_name );
            }
            
            return;
        }
        
        if ( cmd.get_argument(0) == "set_queue_property" )
        {
            if( cmd.get_num_arguments() < 4 )
                error->set( au::str("Not enough parameters for command %s" , main_command.c_str() ) );
            else
            {
                std::string queue_name = prefix + cmd.get_argument(1);
                std::string property   = cmd.get_argument(2);
                std::string value      = cmd.get_argument(3);
                
                stream::Queue *queue = samsonWorker->streamManager->getQueue( queue_name );
                queue->setProperty( property , value );
            }
            return;
        }            

        if ( cmd.get_argument(0) == "unset_queue_property" )
        {
            if( cmd.get_num_arguments() < 3 )
                error->set( au::str("Not enough parameters for command %s" , main_command.c_str() ) );
            else
            {
                std::string queue_name = prefix + cmd.get_argument(1);
                std::string property   = cmd.get_argument(2);
                
                stream::Queue *queue = samsonWorker->streamManager->getQueue( queue_name );
                queue->unsetProperty( property );
            }
            return;
        }            
        
        
        if( ( main_command == "add_stream_operation" ) || ( main_command == "aso"  ) )
        {
            
            au::ErrorManager tmp_error;
            stream::StreamOperation* stream_operation = stream::StreamOperation::newStreamOperation(samsonWorker->streamManager , command , tmp_error );
            
            if( !stream_operation )
                error->set( tmp_error.getMessage() );
            else
                samsonWorker->streamManager->add( stream_operation );
            
            return;
        }
        
        if( main_command == "rm_stream_operation" )
        {
            // Remove a particular stream operation
            
            if( cmd.get_num_arguments() < 2 )
            {
                error->set( "Usage: rm_stream_operation name [-f] " );
                return;
            }
            
            // Get flag force
            bool force = cmd.get_flag_bool("f");
            
            for ( int i = 1 ; i < cmd.get_num_arguments() ; i ++ )
            {
                // New operation to remove
                std::string name = prefix + cmd.get_argument( i );

                // Remove 
                Status s = streamManager->remove_stream_operation( name );
                
                if ( s != OK )
                    if( !force )
                        error->set( au::str("StreamOperation %s does not exist" , name.c_str()  ) );
            }
            return;
            
        }
        
        if( main_command == "set_stream_operation_property" )
        {
            if( cmd.get_num_arguments() < 4 )
            {
                error->set( "Usage: set_stream_operation_property name property value " );
                return;
            }
            
            std::string name            = prefix + cmd.get_argument( 1 );
            std::string property        = cmd.get_argument( 2 );
            std::string value           = cmd.get_argument( 3 );
            
            Status s = streamManager->set_stream_operation_property( name , property, value );
            
            if( s!= OK )
                error->set( au::str("Error in set_stream_operation_property %s" , status(s)  ));
            
            return;
        } 
        
        if( main_command == "unset_stream_operation_property" )
        {
            if( cmd.get_num_arguments() < 3 )
            {
                error->set( "Usage: unset_stream_operation_property name property" );
                return;
            }
            
            std::string name            = prefix + cmd.get_argument( 1 );
            std::string property        = cmd.get_argument( 2 );
            
            Status s = streamManager->unset_stream_operation_property( name , property );
            
            if( s!= OK )
                error->set( au::str("Error in unset_stream_operation_property %s" , status(s)  ));
            
            return;
        } 
        
        error->set( au::str("Unknown command %s" , main_command.c_str()  ) );
        
    }
    
    
    void WorkerCommand::run()
    {
        // Nothing to to if this is waiting for another thing
        if( !pending_to_be_executed )
            return;
        pending_to_be_executed = false; // Not pending any more, except if something happen...
        
        // Parse command
        au::CommandLine cmd;
        cmd.set_flag_boolean("clear_inputs");
        cmd.set_flag_boolean("error");
        cmd.set_flag_boolean("warning");
        cmd.set_flag_boolean("f");
        cmd.set_flag_boolean("new");
        cmd.set_flag_boolean("remove");
        cmd.set_flag_string("group", ""); // Possible parameers
        cmd.set_flag_boolean("save");
        cmd.set_flag_boolean("v");
        cmd.set_flag_boolean("vv");
        cmd.set_flag_boolean("vvvv");
        cmd.set_flag_boolean("in");
        cmd.set_flag_boolean("out");
        cmd.set_flag_boolean("a");
        cmd.set_flag_boolean("properties");
        cmd.set_flag_boolean("rates");
        cmd.set_flag_boolean("blocks");
        cmd.set_flag_boolean("running");
        
        cmd.parse( command );

        // Get visualitzation option ( if any )
        VisualitzationOptions visualitzation_options = normal;
        if( cmd.get_flag_bool("v") )
            visualitzation_options = verbose;
        if( cmd.get_flag_bool("vv") )
            visualitzation_options = verbose2;
        if( cmd.get_flag_bool("vvv") )
            visualitzation_options = verbose3;
        if( cmd.get_flag_bool("a") )
            visualitzation_options = all;
        if( cmd.get_flag_bool("in") )
            visualitzation_options = in;
        if( cmd.get_flag_bool("out") )
            visualitzation_options = out;
        if( cmd.get_flag_bool("properties") )
           visualitzation_options = properties;
        if( cmd.get_flag_bool("rates") )
            visualitzation_options = rates;
        if( cmd.get_flag_bool("blocks") )
            visualitzation_options = blocks;
        if( cmd.get_flag_bool("running") )
            visualitzation_options = running;
        
        if( cmd.get_num_arguments() == 0 )
        {
            finishWorkerTaskWithError("No command provided");
            return;
        }
        
        // Main command
        std::string main_command = cmd.get_argument(0);
        
        // Get pattern with the second argument if necessary
        std::string pattern ="*";
        if( cmd.get_num_arguments() >= 2)
            pattern = cmd.get_argument(1);
        
        // More general way of visualization
        Visualization visualitzation;
        visualitzation.options = visualitzation_options;
        visualitzation.pattern = pattern; 
        
        
        // Query commands
        if( main_command == "ls_queues" )
        {
            network::Collection * c = streamManager->getCollection( visualitzation_options,pattern );
            c->set_title( command  );
            collections.push_back( c );
            finishWorkerTask();
            return;
        }

        if( main_command == "ls_stream_operations" )
        {
            network::Collection * c = streamManager->getCollectionForStreamOperations( visualitzation_options,pattern );
            c->set_title( command  );
            collections.push_back( c );
            finishWorkerTask();
            return;
        }
        
        if( main_command == "ps_stream")
        {
            // Add the ps stream
            network::Collection * c = streamManager->queueTaskManager.getCollection( visualitzation_options , pattern );
            c->set_title( command  );
            collections.push_back( c);
            finishWorkerTask();
            return ;
        }
        
        if( main_command == "ls_workers" )
        {
            // Add the ps stream
            network::Collection * c = samsonWorker->getWorkerCollection( &visualitzation );
            c->set_title( command  );
            collections.push_back( c);
            finishWorkerTask();
            return ;
        }
        
        if ( main_command == "ls_modules" )
        {
            network::Collection * c = ModulesManager::shared()->getModulesCollection( visualitzation_options , pattern ) ;
            c->set_title( command  );
            collections.push_back( c );
            finishWorkerTask();
            return;
        }
        if ( main_command == "ls_operations" )
        {
            network::Collection * c = ModulesManager::shared()->getOperationsCollection( visualitzation_options , pattern ) ;
            c->set_title( command  );
            collections.push_back( c);
            finishWorkerTask();
            return;
        }
        if ( main_command == "ls_datas" )
        {
            network::Collection * c = ModulesManager::shared()->getDatasCollection( visualitzation_options , pattern );
            c->set_title( command  );
            collections.push_back( c );
            finishWorkerTask();
            return;
        }
        
        if( main_command == "ls_connections" )
        {
            network::Collection * c = samsonWorker->network->getConnectionsCollection(&visualitzation);
            if( c )
            {
                c->set_title( command );
                collections.push_back( c );
            }
            finishWorkerTask();
            return;
        }
        
        if( main_command == "ls_worker_commands" )
        {
            network::Collection * c = samsonWorker->workerCommandManager->getCollectionOfWorkerCommands(&visualitzation);
            c->set_title( command  );
            collections.push_back( c );
            finishWorkerTask();
            return;
        }
        
        if( main_command == "show_stream_block" )
        {
            
            std::string pattern_inputs;
            std::string pattern_states;
            std::string pattern_outputs;
            
            std::string path;
            
            if ( cmd.get_num_arguments() >= 2 )
            {
                 path = cmd.get_argument(1);
                
                if( path.substr( path.length() - 1 ) != "/" )
                    path.append("/");
                
                pattern_inputs += path;
                pattern_states += path;
                pattern_outputs += path;
            }
            else
                path = "<root>";

            pattern_inputs += "in:*";
            pattern_states += "state:*";
            pattern_outputs += "out:*";
            
            // Inputs
            network::Collection *c_inputs = streamManager->getCollection( verbose , pattern_inputs );
            c_inputs->set_title( au::str("Inputs for %s", path.c_str()) );
            c_inputs->set_name("inputs");
            collections.push_back( c_inputs );
            
            // States
            network::Collection *c_states = streamManager->getCollection( normal , pattern_states );
            c_states->set_title( au::str("States for %s", path.c_str()) );
            c_states->set_name("states");
            collections.push_back( c_states );
            
            // Outputs
            network::Collection *c_outputs = streamManager->getCollection( verbose , pattern_outputs );
            c_outputs->set_title( au::str("Outputs for %s", path.c_str()) );
            c_outputs->set_name("outputs");
            collections.push_back( c_outputs );

            finishWorkerTask();
            return;
        }
        
        
        if( main_command == "wait" )
        {
            // Special operation to wait until no activity is present in stream manager
            
            if( streamManager->queueTaskManager.isActive() )
                pending_to_be_executed = true;
            else
                finishWorkerTask();
            
            return;
        }
        
        if( main_command == "send_trace")
        {
            std::string message = "No message";
            if( cmd.get_num_arguments() >= 2)
                message = cmd.get_argument(1);

            // Full message
            std::string full_message = au::str("[Message from Delilah_%s] %s" 
                                               , au::code64_str( delilah_id ).c_str() 
                                               , message.c_str() 
                                               );

            
            // Send a trace to all delilahs
            if( cmd.get_flag_bool("error") )
                samsonWorker->sendTrace( "error" , "delilah" , full_message );
            else if( cmd.get_flag_bool("warning") )
                samsonWorker->sendTrace( "warning" , "delilah" , full_message );
            else
                samsonWorker->sendTrace( "message" , "delilah" , full_message );
            
            
            finishWorkerTask();
            return;
        }
        
        if( main_command == "connect_to_queue" )
        {
            if( cmd.get_num_arguments() < 2 )
            {
                finishWorkerTaskWithError( "Usage: connect_to_queue queue (-new) (-remove)" );
                return;
            }
            std::string queue = cmd.get_argument(1);
            
            bool flag_new = cmd.get_flag_bool("new");
            bool flag_remove = cmd.get_flag_bool("remove");
            
            samsonWorker->streamManager->connect_to_queue( delilah_id , queue , flag_new , flag_remove );
            
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
            
            samsonWorker->streamManager->disconnect_from_queue( delilah_id , queue );
            
            finishWorkerTask();
            return;
            
        }
        
        if( main_command == "run_stream_operation" )
        {
            // Flag used in automatic update operation to lock input blocks and remove after operation
            bool clear_inputs =  cmd.get_flag_bool("clear_inputs"); 
            
            // Minimum size to run an operation
            size_t max_size = SamsonSetup::shared()->getUInt64("stream.max_operation_input_size");          
            
            // Collection of block ids for the already processed blocks ( only used in clear_inputs is not activated )
            stream::BlockIdList block_ids;      
            
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
                KVFormat queue_format = samsonWorker->streamManager->getQueue( queue_name )->getFormat();
                
                if( queue_format != KVFormat("*","*") )
                    if( queue_format != operation_format )
                    {
                        finishWorkerTaskWithError( au::str("Format for input %d ( operation %s ) is %s while queue %s has format %s" 
                                                           , i , operation_name.c_str() , operation_format.str().c_str() , queue_name.c_str() , queue_format.str().c_str() ) );
                        return;
                    }
                
            }
            
            
            switch (op->getType()) 
            {
                    
                case Operation::reduce:
                {
                    stream::StreamOperationBase *operation = getStreamOperation( op );
                    
                    // Get the input queues
                    std::vector< stream::Queue* > queues;
                    BlockInfo block_info;
                    int num_divisions = 1;
                    
                    
                    for (int i = 0 ; i < op->getNumInputs() ; i++ )
                    {
                        std::string input_queue_name = operation->input_queues[i];
                        stream::Queue*queue = streamManager->getQueue( input_queue_name );
                        queue->update( block_info );
                        queues.push_back( queue );                            
                    }
                    
                    // For each range, create a set of reduce operations...
                    for ( int r = 0 ; r < num_divisions ; r++ )
                    {
                        KVRange  range = rangeForDivision( r , num_divisions );
                        
                        
                        //Create the reduce operation
                        stream::ReduceQueueTask *tmp = new stream::ReduceQueueTask( streamManager->queueTaskManager.getNewId() , operation , range ); 
                        tmp->addOutputsForOperation(op);
                        
                        // Take data from each input
                        for (int q = 0 ; q < (int) queues.size() ; q++ )
                        {
                            stream::BlockList* list = tmp->getBlockList( au::str("input_%d" , q ) ); 
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
                    
                    stream::StreamOperationBase *operation = getStreamOperation( op );
                    
                    // Get the input queue
                    std::string input_queue_name = operation->input_queues[0];
                    stream::Queue *queue = streamManager->getQueue( input_queue_name );
                    
                    // Change max size to make sure we distribute work over all cores...
                    BlockInfo block_info;
                    queue->update(block_info);
                    size_t tmp_max_size = block_info.size / SamsonSetup::shared()->getInt("general.num_processess");
                    if( tmp_max_size < max_size )
                        max_size = tmp_max_size;
                    
                    
                    stream::BlockList inputBlockList;
                    if( clear_inputs )
                        inputBlockList.extractFrom( queue->list , 0 );
                    else
                        inputBlockList.copyFrom( queue->list ,  0 );
                    
                    while( !inputBlockList.isEmpty() )
                    {
                        
                        // Get a BlockList with content to be processed
                        stream::BlockList inputData("input data");
                        inputData.extractFrom( &inputBlockList , max_size );
                        
                        // Get a new id for the next operation
                        size_t id = streamManager->queueTaskManager.getNewId();
                        
                        stream::QueueTask *tmp = NULL;
                        switch ( op->getType() ) 
                        {
                            case Operation::parser:
                            {
                                tmp = new stream::ParserQueueTask( id , operation ); 
                            }
                                break;
                                
                            case Operation::map:
                            {
                                tmp = new stream::MapQueueTask( id , operation , KVRange(0,KVFILE_NUM_HASHGROUPS) ); 
                            }
                                break;
                                
                            case Operation::parserOut:
                            {
                                tmp = new stream::ParserOutQueueTask( id , operation , KVRange(0,KVFILE_NUM_HASHGROUPS) ); 
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
        
        // Simple commands
        au::ErrorManager error;
        runCommand(  command, &error); // No prefix here
        
        if( error.isActivated() )
            finishWorkerTaskWithError( error.getMessage() ); 
        else
            finishWorkerTask();
        
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
            p->message->set_delilah_component_id( delilah_component_id );
            
            // Direction of this packets
            p->to.node_type = DelilahNode;
            p->to.id = delilah_id;
            
            
            // Add collections as answers...
            for ( size_t i = 0 ; i < collections.size() ; i++ )
            {
                p->message->add_collection()->CopyFrom( *collections[i] );
            }
            
            // Send the packet
            samsonWorker->network->send( p );
        }
        
        // Set the finished flag
        finished = true;
        
    }
    
    stream::StreamOperationBase *WorkerCommand::getStreamOperation( Operation *op )
    {
        // Parsing the global command
        au::CommandLine cmd;
        cmd.parse(command);
        
        int pos_argument = 1;   // We skip the "run_stream_operation" parameter
        
        std::string operation_name = cmd.get_argument(pos_argument++);
        
        // Disitribution informaiton for this stream operation
        DistributionInformation distribution_information;
        distribution_information.workers = samsonWorker->network->getWorkerIds();
        distribution_information.network = samsonWorker->network;
        
        stream::StreamOperationBase *operation = new stream::StreamOperationBase( operation_name , distribution_information );
        
        for (int i = 0 ; i < op->getNumInputs() ; i++)
            operation->input_queues.push_back( cmd.get_argument( pos_argument++ ) );
        for (int i = 0 ; i < op->getNumOutputs() ; i++)
            operation->output_queues.push_back( cmd.get_argument( pos_argument++ ) );
     
        // Reasign the name for better description
        operation->name = au::str("run_%s_delilah_%s_%lu" , op->getName().c_str() , au::code64_str(delilah_id).c_str() , delilah_component_id );
        
        
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
    
    void WorkerCommand::fill( samson::network::CollectionRecord* record , Visualization* visualization )
    {
        add( record , "id"      , au::str("Delilah %lu (%lu)", delilah_id , delilah_component_id) , "left,different" );

        if( finished )
            add( record , "status" , "finished" , "left,different" );
        else if ( num_pending_processes > 0 )
            add( record , "status" , au::str("Waiting %lu ops", num_pending_processes) , "f=uint64,sum" );
        else
            add( record , "status" , "running" , "left,different" );

        add( record , "command" , command , "left,different" );
        add( record , "error"   , error.getMessage() , "left,different" );
    }

    
}
