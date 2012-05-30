
#include <algorithm>

#include "au/utils.h"
#include "au/ErrorManager.h"
#include "au/string.h"
#include "au/log/LogToServer.h"

#include "engine/Notification.h"
#include "engine/ProcessManager.h"  
#include "engine/DiskManager.h"


#include "samson/common/EnvironmentOperations.h"        // copyEnvironment
#include "samson/common/SamsonSetup.h"                  // SamsonSetup

#include "samson/network/Packet.h"                      // samson::Packet

#include "samson/worker/SamsonWorker.h"

#include "samson/stream/StreamManager.h"                // samson::stream::StreamManager
#include "samson/stream/Queue.h"
#include "samson/stream/BlockList.h"
#include "samson/stream/BlockManager.h"
#include "samson/stream/StreamOperation.h"
#include "samson/stream/BlockBreakQueueTask.h"

#include "samson/stream/QueueTasks.h"
#include "samson/stream/Block.h"
#include "samson/stream/BlockList.h"

#include "WorkerCommand.h"     // Own interface

namespace samson {
    
    class KVRangeAndSize
    {
    public:
        
        KVRange range;
        size_t size;
        
        KVRangeAndSize( KVRange _range , size_t _size )
        {
            range = _range;
            size = _size;
        }
    };
    
    class KVRangeAndSizeManager
    {
        
        public:
        
        std::vector<KVRangeAndSize> items;  // Input information
        std::vector<KVRange> ranges;        // Output ranges
        
        au::ErrorManager error;             // Error management
        
        void compute_ranges( size_t max_size )
        {
            
            int num_cores = SamsonSetup::shared()->getInt("general.num_processess");
            
            int hg_begin = 0;
            int hg_end = 0;
            
            while( true )
            {
                if( hg_begin >= KVFILE_NUM_HASHGROUPS )
                    break; // No more ranges

                while( 
                      (hg_end < KVFILE_NUM_HASHGROUPS) 
                      && 
                      (compute_size( KVRange(hg_begin,hg_end+1) ) < max_size ) 
                      && ( (hg_end - hg_begin) < (KVFILE_NUM_HASHGROUPS/num_cores) ) // Distribute small reducers...
                      )
                    hg_end++;
                
                if( hg_begin == hg_end )
                {
                    // Error sice it is not possible to include this hash-group alone
                    size_t size = compute_size( KVRange( hg_begin , hg_begin+1) );
                    error.set( au::str("Not possible to process hash-group %d ( %s > %s ). Please degrag input queues"
                                       , hg_begin
                                       , au::str(size).c_str()
                                       , au::str(max_size).c_str() ));
                    return;
                }
                
                // Create a new set
                KVRange r( hg_begin , hg_end );
                ranges.push_back(r);
                
                // Next hash-groups
                hg_begin = hg_end;
                hg_end = hg_begin;
            }
            
        }
        
        size_t compute_size( KVRange range )
        {
            size_t total = 0;
            for( size_t i = 0 ; i < items.size() ; i++ )
                if( items[i].range.overlap( range ) )
                    total += items[i].size; 
            return total;
        }
        
        
    };
    
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
    
    WorkerCommand::WorkerCommand( std::string _worker_command_id
                                 , size_t _delilah_id 
                                 , size_t _delilah_component_id 
                                 ,  const network::WorkerCommand& _command )
    {
        // Unique identifier of the worker
        worker_command_id = _worker_command_id;
        
        samsonWorker = NULL;
        
        //Identifiers to notify when finished
        delilah_id = _delilah_id;
        delilah_component_id = _delilah_component_id;
        
        notify_finish = ( _delilah_id != 0 ); // 0 used as a non notify delilah
        
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
        flush_queues = false;
        
        // No pending process at the moment
        num_pending_processes = 0;
        num_pending_disk_operations = 0;
    }
    
    WorkerCommand::~WorkerCommand()
    {
        if( originalWorkerCommand )
            delete originalWorkerCommand;
        
        // Remove collections created for this command
        collections.clearVector();
        
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
        cmd.set_flag_boolean("flush");
        
        cmd.set_flag_boolean("new");
        cmd.set_flag_boolean("remove");
        cmd.set_flag_string("prefix", "");
        cmd.parse( command );
        
        flush_queues = cmd.get_flag_bool("flush");
        
        std::string prefix = cmd.get_flag_string("prefix");
        
        if ( cmd.get_num_arguments() == 0 )
        {
            error->set("No command provided");
            return;
        }
        
        // Set the main command
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
                prefix.append(".");
                
                operation_name  = cmd.get_argument(2);
            }
            else if( cmd.get_num_arguments() == 2 )
            {
                operation_name  = cmd.get_argument(1);
            }
            
            Operation *op = ModulesManager::shared()->getOperation(  operation_name );
            if( !op )
            {
                error->set( au::str("Unknown operation:'%s' in command arguments to init_stream" , operation_name.c_str() ) );
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
        
        if( main_command == "set_log_server" )
        {
            if( cmd.get_num_arguments() < 2 )
            {
                error->set( "Usage: set_log_server host [port]" );
                return;
            }
            
            if( cmd.get_num_arguments() == 2 )
            {
                std::string host = cmd.get_argument(1);
                au::set_log_server( host );
                return;
            }
            
            if( cmd.get_num_arguments() > 2 )
            {
                std::string host = cmd.get_argument(1);
                int port = atoi( cmd.get_argument(2).c_str() );
                au::set_log_server( host , port );
                return;
            }
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
        
        if ( main_command == "rm" )
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


        
        if ( cmd.get_argument(0) == "push_queue" )
        {
            if( cmd.get_num_arguments() < 3 )
                error->set( au::str("Not enough parameters for command %s" , main_command.c_str() ) );
            else
            {
                std::string from_queue_name = prefix + cmd.get_argument(1);
                std::string to_queue_name   = prefix + cmd.get_argument(2);
                samsonWorker->streamManager->push_queue(  from_queue_name , to_queue_name );
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

        if( ( main_command == "rm_queue_connection" ) )
        {
            if( cmd.get_num_arguments() < 3 )
            {
                error->set( "Usage: rm_queue_connection queue target_queue_1 target_queue_2 .. target_queue_N" );
                return;
            }
            
            std::string source_queue = cmd.get_argument(1);
            for( int i = 2 ; i < cmd.get_num_arguments() ; i++ )
            {
                std::string target_queue = cmd.get_argument(i);
                
                if( target_queue != source_queue )     // Avoid connecting with yourself            
                    streamManager->remove_queue_connection( source_queue , target_queue );
            }
            return;

        }
        
        
        if( ( main_command == "add_queue_connection" ) )
        {
            if( cmd.get_num_arguments() < 3 )
            {
                error->set( "Usage: add_queue_connection source_queue target_queue_1 target_queue_2 .. target_queue_N" );
                return;
            }
            
            std::string source_queue = prefix + cmd.get_argument(1);
            for( int i = 2 ; i < cmd.get_num_arguments() ; i++ )
            {
                std::string target_queue = prefix + cmd.get_argument(i);
                
                if( target_queue != source_queue )     // Avoid connecting with yourself            
                    streamManager->add_queue_connection( source_queue , target_queue );
            }
            
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
                error->set( au::str("Error (%s) setting property '%s' with value '%s' for stream operation %s" 
                                    , status(s)
                                    , property.c_str()
                                    , value.c_str()
                                    , name.c_str()
                                    ));
            
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
    
    
    bool compare_blocks_defrag( stream::Block* b , stream::Block* b2 )
    {
        
        KVHeader h1 = b->getHeader();
        KVHeader h2 = b2->getHeader();

        if( h1.range.hg_begin < h2.range.hg_begin )
            return true;
        if( h1.range.hg_begin > h2.range.hg_begin )
            return false;

        // If the same 
        if( h1.range.hg_end > h2.range.hg_end )
            return true;
        else
            return false;
    }
    


    static bool logFilter
    (
        const char*  filter,
        const char*  all,
        char         type,
        const char*  fileName
    )
    {
        std::string  item;
        char*        value;
        char*        eq;
        char         filterCopy[512];

        strncpy(filterCopy, filter, sizeof(filterCopy));

        eq = strchr((char*) filterCopy, '=');
        if (eq != NULL)
        {
            *eq = 0;
            ++eq;
            item  = std::string(filterCopy);
            value = eq;
        }
        else
        {
            item  = std::string("XXXXX_NO_ITEM");
            value = (char*) filterCopy;
        }

        // No item - grep in all
        if (item == "XXXXX_NO_ITEM")
        {
            if (strstr(all, value) != NULL)
                return true;
            return false;
        }

        if (item == "Type")
        {
            if (strlen(value) != 1)
                return false;

            if (value[0] != type)
                return false;

            return true;
        }

        if (item == "FileName")
        {
            if (strncmp(value, fileName, strlen(value)) == 0)
                return true;

            return false;
        }

        return false;
    }


typedef struct LogLineInfo
{
    char         type;
    std::string  date;
    int          ms;
    std::string  progName;
    std::string  fileName;
    int          lineNo;
    int          pid;
    int          tid;
    std::string  funcName;
    std::string  message;
} LogLineInfo;

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
        cmd.set_flag_boolean("disk");
        cmd.set_flag_boolean("engine");
        cmd.set_flag_int("lines", 20);
        
        // Flags used in delilah side...
        cmd.set_flag_boolean("hidden");
        cmd.set_flag_boolean("save");     // Flag to identify if is necessary to save it locally
        cmd.set_flag_boolean("connected_workers");    // Flag to run the operation only with connected workers
        cmd.set_flag_uint64("worker" , (size_t)-1 );
        cmd.set_flag_string("group", "");
        cmd.set_flag_string("filter", "");
        cmd.set_flag_string("sort", "");
        cmd.set_flag_uint64("limit" , 0 );
        cmd.set_flag_string("c", "");  // Command in run operations
        
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
        if( cmd.get_flag_bool("disk") )
            visualitzation_options = disk;
        if( cmd.get_flag_bool("engine") )
            visualitzation_options = engine;
        
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
        
        if( main_command == "flush_buffers" )
        {
            streamManager->flushBuffers();
            finishWorkerTask();
            return;
        }
        
        if ( main_command == "push_module" )
        {
            if( cmd.get_num_arguments() < 2 )
            {
                finishWorkerTaskWithError( "Usage push_module <name>" );
                return;
            }
            
            engine::Buffer * buffer = buffer_container.getBuffer();
            
            if ( !buffer )
            {
                finishWorkerTaskWithError("No data provided for this module");
                return;
            }
            
            // Destination file
            std::string file_name = au::str("%s/lib%s.so" 
                                            , SamsonSetup::shared()->modulesDirectory().c_str() 
                                            , cmd.get_argument(1).c_str() 
                                            );
            
            engine::DiskOperation *operation = engine::DiskOperation::newWriteOperation(buffer, file_name, getEngineId() );
            operation->environment.set("push_module", "yes");
            engine::DiskManager::shared()->add( operation );
            operation->release(); // It is now retained by disk manager
            
            num_pending_disk_operations++;
            
            // Buffer will be destroyed by the disk operation
            buffer = NULL;
            
            return;
        }        
        
        // Query commands
        if( main_command == "ls" )
        {
            network::Collection * c = streamManager->getCollection( &visualitzation );
            c->set_title( command  );
            collections.push_back( c );
            finishWorkerTask();
            return;
        }

        if( main_command == "ls_blocks" )
        {
            network::Collection * c = stream::BlockManager::shared()->getCollectionOfBlocks( &visualitzation );
            c->set_title( command  );
            collections.push_back( c );
            finishWorkerTask();
            return;
        }

        if( main_command == "ls_buffers" )
        {
            network::Collection * c = getCollectionOfBuffers( &visualitzation );
            c->set_title( command  );
            collections.push_back( c );
            finishWorkerTask();
            return;
        }
        
        if( main_command == "ls_pop_connections" )
        {
            network::Collection * c = streamManager->getCollectionForPopConnections(&visualitzation);
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

        if( main_command == "ls_queues_connections" )
        {
            network::Collection * c = streamManager->getCollectionForQueueConnections( &visualitzation  );
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
        
        if( main_command == "ps_workers" )
        {
            network::Collection * c = samsonWorker->workerCommandManager->getCollectionOfWorkerCommands(&visualitzation);
            c->set_title( command  );
            collections.push_back( c );
            finishWorkerTask();
            return;
        }
        
        if( main_command == "show_stream_block" )
        {
            std::string path;
            
            if ( cmd.get_num_arguments() >= 2 )
            {
                path = cmd.get_argument(1);
                
                if( path.substr( path.length() - 1 ) != "." )
                    path.append(".");
                
            }
            else
                path = "";

            
            network::Collection * c = streamManager->getCollectionForStreamBlock( path , &visualitzation);
            c->set_title( command  );
            collections.push_back( c );
            finishWorkerTask();
            return;
            
            // Internal stream blocks

            
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
        
        if (main_command == "log")
        {
            char                          type;
            char                          date[64];
            int                           ms;
            char                          progName[64];
            char                          fileName[64];
            int                           lineNo;
            int                           pid;
            int                           tid;
            char                          funcName[64];
            char                          message[256];
            int                           hits   = 0;
            std::list<LogLineInfo>        lines;
            LogLineInfo                   logLine;
            std::string                   filter     = cmd.get_argument(1);
            int                           maxLines   = cmd.get_flag_int("lines");
            long                          lmPos      = 0;
            char*                         all        = NULL;

            while (1) // Until lmLogLineGet returns EOF (-2)
            {
                if (all != NULL) // strdup and free - a little slow ... I might replace this mechanism with a char[] ...
                    free(all);
                
                lmPos = lmLogLineGet(&type, date, &ms, progName, fileName, &lineNo, &pid, &tid, funcName, message, lmPos, &all);
                if (lmPos < 0)
                {
                    if (all != NULL)
                        free(all);
                    break;
                }

                if (filter != "no-argument")
                {
                    if (logFilter(filter.c_str(), all, type, fileName) == false)
                        continue;
                }

                logLine.type          = type;
                logLine.date          = date;
                logLine.ms            = ms;
                logLine.progName      = progName;
                logLine.fileName      = fileName;
                logLine.lineNo        = lineNo;
                logLine.pid           = pid;
                logLine.tid           = tid;
                logLine.funcName      = funcName;
                logLine.message       = message;

                lines.push_front(logLine);

                ++hits;
                if (maxLines != 0)
                {
                    if ((int) lines.size() > maxLines)
                        lines.pop_back();
                }
            }

            // Close fP for log file peeking ...
            lmPos = lmLogLineGet(NULL, (char*) date, &ms, progName, fileName, &lineNo, &pid, &tid, funcName, message, lmPos, NULL);

            samson::network::Collection*  collection = new samson::network::Collection();
            collection->set_name("Log File Lines");
            collection->set_title("Log File Lines");

            while (lines.size() > 0)
            {
                samson::network::CollectionRecord* record = collection->add_record();

                logLine = lines.back();
                lines.pop_back();

                ::samson::add(record, "Type",         logLine.type,     "left,different");
                ::samson::add(record, "Date",         logLine.date,     "left,different");
                ::samson::add(record, "Milliseconds", logLine.ms,       "left,different");
                ::samson::add(record, "ProgramName",  logLine.progName, "left,different");
                ::samson::add(record, "FileName",     logLine.fileName, "left,different");
                ::samson::add(record, "LineNo",       logLine.lineNo,   "left,different");
                ::samson::add(record, "PID",          logLine.pid,      "left,different");
                ::samson::add(record, "TID",          logLine.tid,      "left,different");
                ::samson::add(record, "Function",     logLine.funcName, "left,different");
                ::samson::add(record, "Message",      logLine.message,  "left,different");
            }

            collections.push_back(collection);

            finishWorkerTask();
            return;
        } 

        if (main_command == "trace")
        {
            std::string subcommand;
            std::string levels;

            //
            // 01: off
            // 02: get
            // 03: set [09-,]*
            // 04: add [09-,]*
            // 05: del [09-,]*
            //
            if (cmd.get_num_arguments() < 2)
            {
                finishWorkerTaskWithError( "Usage: trace [ off ] [ get ] [ set | add | del (range-list of trace levels)]" );
                return;
            }                

            subcommand = cmd.get_argument(1);

            if (subcommand == "off")
                lmTraceSet(NULL);
            else if (subcommand == "get")
            {
                char levels[1024];

                lmTraceGet(levels);
                
                
                samson::network::Collection* collection = new  samson::network::Collection();
                collection->set_title("Trace information");
                collection->set_name("trace_levels");
                
                samson::network::CollectionRecord* record = collection->add_record();
                
                ::samson::add(record, "Trace levels", levels, "left,different" );
                collections.push_back(collection);
                
                finishWorkerTask();
                return;
            }
            else if ((subcommand == "set") || (subcommand == "add") || (subcommand == "del"))
            {
                if (cmd.get_num_arguments() < 3)
                {
                    finishWorkerTaskWithError( "Usage: trace " + subcommand + " (range-list of trace levels)");
                    return;
                }                

                levels = cmd.get_argument(2);

                if (subcommand == "set")
                    lmTraceSet((char*) levels.c_str());
                else if (subcommand == "add")
                    lmTraceAdd((char*) levels.c_str());
                else if (subcommand == "del")
                    lmTraceSub((char*) levels.c_str());
            }
            else
            {
                finishWorkerTaskWithError( au::str("Usage: bad subcommand for 'trace': %s", subcommand.c_str()));
                return;
            }

            finishWorkerTask();
            return;
        }

        if (main_command == "wverbose")
        {
            std::string subcommand;

            if (cmd.get_num_arguments() < 2)
            {
                finishWorkerTaskWithError( "Usage: verbose [ off ] [ 0 - 5 ]");
                return;
            }                

            subcommand = cmd.get_argument(1);

            if ((subcommand == "off") || (subcommand == "0"))
            {
                lmVerbose  = false;
                lmVerbose2 = false;
                lmVerbose3 = false;
                lmVerbose4 = false;
                lmVerbose5 = false;
            }
            else if ((subcommand == "1") || (subcommand == "2") || (subcommand == "3") || (subcommand == "4") || (subcommand == "5"))
            {
                int level = subcommand.c_str()[0] - '0';

                switch (level)
                {
                case 5: lmVerbose5 = true;
                case 4: lmVerbose4 = true;
                case 3: lmVerbose3 = true;
                case 2: lmVerbose2 = true;
                case 1: lmVerbose  = true;
                }
            }
            else
            {
                finishWorkerTaskWithError( "Usage: verbose [ off ] [ 0 - 5 ]");
                return;
            }

            finishWorkerTask();
            return;
        }

        if (main_command == "wdebug")
        {
            std::string subcommand;

            if (cmd.get_num_arguments() < 2)
            {
                finishWorkerTaskWithError( "Usage: wdebug [ off | on ]");
                return;
            }                

            subcommand = cmd.get_argument(1);

            if (subcommand == "off")
                lmDebug = false;
            else if (subcommand == "on")
                lmDebug = true;
            else
            {
                finishWorkerTaskWithError( "Usage: wdebug [ off | on ]");
                return;
            }

            finishWorkerTask();
            return;
        }

        if( main_command == "send_alert")
        {
            std::string message = "No message";
            if( cmd.get_num_arguments() >= 2)
                message = cmd.get_argument(1);

            // Full message
            std::string full_message = au::str("[Alert from Delilah_%s] %s" 
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

            // Set finish if no pending task
            if( num_pending_processes == 0 )
                finishWorkerTask();
            return;
            
        }

        if( main_command == "defrag" )
        {
            if( cmd.get_num_arguments() < 3 )
            {
                finishWorkerTaskWithError( "Usage: defrag queue_source queue_destination" );
                return;
            }
            
            std::string queue_from = cmd.get_argument(1);
            std::string queue_to   = cmd.get_argument(2);

            // Pointer to the queue we want to defrag
            stream::Queue*queue = streamManager->getQueue( queue_from );
                        
            // Create information about how data is distributed in files to do the best defrag
            stream::BlockList init_list("defrag_block_list");
            init_list.copyFrom( queue->list );
            
            size_t input_operation_size = SamsonSetup::shared()->getUInt64("general.memory") / 3;
            
            size_t num_defrag_blocks = init_list.getBlockInfo().size / input_operation_size;
            if( num_defrag_blocks == 0)
                num_defrag_blocks = 1;
            size_t output_operation_size = input_operation_size / num_defrag_blocks;
            
            if( output_operation_size > (64*1024*1024) )
                output_operation_size = (64*1024*1024);
            
            while( true )
            {
                {
                    stream::BlockList tmp_list("defrag_block_list");
                    tmp_list.extractFromForDefrag( &init_list , input_operation_size );
                
                    if( tmp_list.isEmpty() )
                        break;

                    
                    // Create an operation to process this set of blocks
                    size_t new_id = streamManager->queueTaskManager.getNewId();
                    stream::BlockBreakQueueTask *tmp = new stream::BlockBreakQueueTask( new_id , queue_to , output_operation_size ); 
                    
                    // Fill necessary blocks
                    tmp->getBlockList( au::str("input" ) )->copyFrom( &tmp_list );
                    
                    // Set the working size to get statistics at ProcessManager
                    tmp->setWorkingSize();
                    
                    // Add me as listener and increase the number of operations to run
                    tmp->addListenerId( getEngineId() );
                    num_pending_processes++;
                    
                    tmp->environment.set("worker_command_id", worker_command_id);
                    
                    // Schedule tmp task into QueueTaskManager
                    streamManager->queueTaskManager.add( tmp );       
                }
            }

            return;
        }
       
        if( main_command == "cancel_stream_operation" )
        {
            if( cmd.get_num_arguments() < 2 )
            {
                finishWorkerTaskWithError( au::str("Not enough parameters for command %s\nUsage: cancel_stream_operation operation_id" , main_command.c_str() ) );
                return;
            }
            
            std::string canceled_worker_command_id = cmd.get_argument(1);
            
            // Emit a cancelation notification to cancel as much cas possible
            engine::Notification * notification = new engine::Notification("cancel");
            notification->environment.set("id", canceled_worker_command_id );

            // This is a stream operation ( not return error ;) )
            std::string prefix = "stream_";
            if( canceled_worker_command_id.substr( 0 , prefix.length() ) == prefix )
                finishWorkerTask();
            
            // Remove in the worker command manager
            if( samsonWorker->workerCommandManager->cancel( canceled_worker_command_id ) )
                finishWorkerTask();
            else
                finishWorkerTaskWithError(au::str("Worker command %s not found" , canceled_worker_command_id.c_str() ));
            
            return;
        }
        
        if( main_command == "run" )
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
                finishWorkerTaskWithError( au::str("Not enough parameters for command '%s'" , main_command.c_str() ) );
                return;
            }
            
            std::string operation_name = cmd.get_argument(1);
            
            Operation *op = ModulesManager::shared()->getOperation(  operation_name );
            if( !op )
            {
                finishWorkerTaskWithError( au::str("Unknown operation:'%s' in command arguments to run" , operation_name.c_str() ) );
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
                        finishWorkerTaskWithError( au::str("Format for input %d ( operation '%s' ) is %s while queue %s has format %s"
                                                           , i , operation_name.c_str() , operation_format.str().c_str() , queue_name.c_str() , queue_format.str().c_str() ) );
                        return;
                    }
                
            }
            
            
            // Generate the environment from extra parameters
            Environment operation_environment;
            for ( size_t i = ( 2 + ( op->getNumInputs() + op->getNumOutputs() )) ; i < (size_t)cmd.get_num_arguments() ; i++ )
            {
                std::string full_name = cmd.get_argument(i);
                std::string prefix = "env:";
                if( full_name.substr(0,prefix.size()) == prefix  )
                {
                    std::string name = full_name.substr( prefix.size() );
                    if( i <= (size_t) (cmd.get_num_arguments()-2 ) )
                    {
                        // Set property...
                        std::string value = cmd.get_argument(i+1);
                        operation_environment.set(name , value );
                    }
                }
            }
            
            // Command environment directly in command line with "-c" option
            std::string cmd_command = cmd.get_flag_string("c");
            if( cmd_command != "" )
                operation_environment.set("command" , cmd_command );
            
            
            switch (op->getType()) 
            {
                    
                case Operation::reduce:
                {
                    stream::StreamOperationBase *operation = getStreamOperation( op );
                    
                    
                    size_t memory = SamsonSetup::shared()->getUInt64("general.memory");
                    //int num_cores = SamsonSetup::shared()->getInt("general.num_processess");

                    //size_t max_memory =  memory / num_cores;
                    size_t max_memory =  memory / 2;

                    // Compute the ranges to use
                    KVRangeAndSizeManager ranges_manager;
                    
                    // Add all input blocs to the ranges_manager
                    std::vector< stream::Queue* > queues;
                    for (int i = 0 ; i < op->getNumInputs() ; i++ )
                    {
                        std::string input_queue_name = operation->input_queues[i];
                        stream::Queue*queue = streamManager->getQueue( input_queue_name );
                        queues.push_back( queue );                            
                        
                        au::list< stream::Block >::iterator it_blocks;
                        for( it_blocks = queue->list->blocks.begin() ; it_blocks != queue->list->blocks.end() ; it_blocks++ )
                        {
                            stream::Block* block = *it_blocks;
                            
                            KVRange range = block->getKVRange();
                            size_t size = block->getSize();
                            
                            ranges_manager.items.push_back( KVRangeAndSize( range, size) );
                        }
                    }
                    
                    // Compute used ranges
                    ranges_manager.compute_ranges( max_memory );
                    if ( ranges_manager.error.isActivated() )
                    {
                        finishWorkerTaskWithError( ranges_manager.error.getMessage() ); 
                        return;
                    }

                    
                    // For each range, create a set of reduce operations...
                    for ( size_t r = 0 ; r < ranges_manager.ranges.size() ; r++ )
                    {
                        // Selected ranges
                        KVRange  range = ranges_manager.ranges[r];
                        
                        //Create the reduce operation
                        stream::ReduceQueueTask *tmp = new stream::ReduceQueueTask(
                                                        streamManager->queueTaskManager.getNewId() 
                                                        , operation 
                                                        , range ); 
                        
                        tmp->addOutputsForOperation(op);
                        tmp->operation_environment.copyFrom(&operation_environment);
                        
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
                        
                        // Add to the vector of pending operations
                        // Mark as a new pending operation
                        num_pending_processes++;
                        
                        tmp->environment.set("worker_command_id", worker_command_id);

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

                        // Set enviroment variables
                        tmp->operation_environment.copyFrom(&operation_environment);

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
                        
                        tmp->environment.set("worker_command_id", worker_command_id);
                        
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
                    finishWorkerTaskWithError( au::str("'run' command OK, but it is still not implemented (type:%s)", op->getTypeName().c_str()  ) );
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
        
        // Notify everything so it is automatically canceled
        engine::Notification * notification = new engine::Notification("cancel");
        notification->environment.set("id", worker_command_id );
        engine::Engine::shared()->notify(notification);
    }
    
        
    void WorkerCommand::finishWorkerTask()
    {
        if( finished )
        {
            // If this worker command is finished, not do anything else again.
            return;
        }
        
        // Flush all generated buffers if necessary ( only in run-like commands )
        if( flush_queues )
            streamManager->flushBuffers();
        
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
                p->message->add_collection()->CopyFrom( *collections[i] );
            
            // Send the packet
            samsonWorker->network->send( p );
            
            // Release created packet
            p->release();
        }
        
        // Set the finished flag
        finished = true;
        
    }
    
    stream::StreamOperationBase *WorkerCommand::getStreamOperation( Operation *op )
    {
        // Parsing the global command
        au::CommandLine cmd;
        cmd.parse(command);
        
        int pos_argument = 1;   // We skip the "run" parameter
        
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
        
        if( notification->isName( notification_disk_operation_request_response ) )
        {
            engine::DiskOperation* operation = (engine::DiskOperation*) notification->getObject();
            
            num_pending_disk_operations--;
            if( notification->environment.isSet("error") )
                error.set( notification->environment.get("error" , "no_error") );
                        
            // In case of push module, just reload modules
            if( operation->environment.get("push_module", "no") == "yes" )
            {
                ModulesManager::shared()->reloadModules();
            }
                        
            checkFinish();
            return;
        }
        
        LM_W(("Unexpected notification at WorkerCommand"));
        
    }
    
    void WorkerCommand::checkFinish()
    {
        if( error.isActivated() )
            finishWorkerTask();
        else if( ( num_pending_processes <= 0 ) && (num_pending_disk_operations <= 0 ) )
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
        std::string name = NodeIdentifier( DelilahNode , delilah_id ).getCodeName();
        
        add( record , "id" ,  worker_command_id , "left,different" );

        if( finished )
            add( record , "status" , "finished" , "left,different" );
        else
            add( record , "status" , "running" , "left,different" );

        add( record , "command" , command , "left,different" );
        add( record , "#operations" , num_pending_processes , "left,uint64,sum" );
        add( record , "#disk_operations" , num_pending_disk_operations , "left,uint64,sum" );
        add( record , "error"   , error.getMessage() , "left,different" );

        if (visualization == NULL)
            return;
    }

    network::Collection* WorkerCommand::getCollectionOfBuffers( Visualization* visualization )
    {
        
        network::Collection* collection = new network::Collection();
        collection->set_name("buffers");
        
        au::tables::Table table = engine::MemoryManager::shared()->getTableOfBuffers();
        
        // Debug
        //printf("%s\n" , table.str().c_str() );
        
        for ( size_t r = 0 ; r < table.getNumRows() ; r ++ )
        {
            network::CollectionRecord* record = collection->add_record();
            
            for ( size_t c = 0 ; c < table.getNumColumns() ; c++ )
            {
                std::string concept = table.getColumn(c);
                std::string format = table.getFormatForColumn(c);
                
                std::string value = table.getValue(r, c);
                
                ::samson::add( record , concept , value , format );
            }
        }
        
        return collection;            
        
        
    }
    
    void WorkerCommand::setBuffer( engine::Buffer * buffer )
    {
        buffer_container.setBuffer(buffer);
    }

}
