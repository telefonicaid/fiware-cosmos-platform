

#include "DelilahCommandCatalogue.h" // Own interface

namespace samson
{
    DelilahCommandCatalogue::DelilahCommandCatalogue()
    {
        
        // DELILAH
        // ------------------------------------------------------------------
        
        add( "help","delilah","Get some help about SAMSON platform",
            "help                        Get general help\n"
            "help all [-category cat]    Get all available commands ( optional category )\n" 
            "help command                Get help for a particular command\n"
            );

        add( "connect","delilah",
            "Connect to a SAMSON system",
            "connect host [-port X] [-user X] [-password X]"
            );
        
        add( "disconnect","delilah",
            "Disconnect from a SAMSON system",
            "disconnect"
            );
        
        add( "alerts" , "delilah", 
            "Activate or disactivate showing alerts from SAMSON platform on console",
            "alerts <on> <off>"
            );
        
        add( "local_logs" , "delilah", 
            "Activate or disactivate logs for this delilah",
            "local_logss <on> <off>"
            );

        
        add( "set_log_server" , "delilah",
            "Set log server for all workers",
            "set_log_server host [port]"
            );
        
        add( "log" , "delilah", 
             "log - view lines of the log files of the workers (last 20 lines by default - change using option '-lines')",
             "  log 'pattern'     only lines that match 'pattern'\n"
             "  log Type=X        only lines of Type 'X'\n"
             "  log FileName=X    only lines from the file X"
            );
        
        add( "show_alerts" , "delilah", 
            "Show the last alerts received from SAMSON cluster"
            );
        
        add( "open_alerts_file" , "delilah", 
            "Open a local file to store all received alerts",
            "open_alerts_file [file_name]"
            );
        
        add( "close_alerts_file" , "delilah", 
            "Close local file opened to store with command 'open_traces_file'"
            );
        
        add( "verbose" , "delilah", 
            "Activate or disactivate verbose mode. This shows extra information for commands executed in delilah",
            "verbose <on> <off>"
            );

        add( "wlog" , "delilah", 
            "Show information about what logs are activated at samson nodes. Debug, Writes, Reads, Traces, etc",
            "wlog\n"
            );
        
        add( "wverbose" , "delilah", 
            "Activate or disactivate verbose mode for workers. This shows extra information for worker commands",
            "verbose <0-5>\n"
            "verbose off\n"
            "verbose get\n"
            );
        
        add( "wdebug" , "delilah", 
             "Activate or disactivate debug mode for workers. This shows extra information for worker commands",
             "debug on\n"
             "debug off\n"
             "debug get\n"
            );
        
        add( "wreads" , "delilah", 
            "Activate or disactivate reads mode for workers. This shows extra information for worker commands",
            "reads on\n"
            "reads off\n"
            "reads get\n"
            );
        
        add( "wwrites" , "delilah", 
            "Activate or disactivate writes mode for workers. This shows extra information for worker commands",
            "writes <on> <off> <get>"
            );
        
        add( "wtrace", "delilah",
             "Activate or disactivate trace levels for workers (trace level format example: '1-5,7,76-99'). This shows extra information for worker commands",
             "wtrace off\n"
             "wtrace get\n"
             "wtrace set <levels>\n"
             "wtrace add <levels>\n"
             "wtrace remove <levels>\n");

        add( "send_alert" , "delilah", 
            "Send an alert to all connected delilahs using a random worker as sender",
            "send_alert [-worker X] [-error] [-warning] \"Message to be sent\"\n"
            "      -worker X     Use only this worker as broadcaster\n"
            "      -error        Mark this trace as an error for correct visualization\n"
            "      -warning      Mark this trace as a warning for correct visualization\n"
            "      message       The text message of this alert\n"
            );

        /*
        add( "repeat" , "delilah",
            "Repeat a command continuously",
            "repeat <command>. Stop them using stop_repeat\n\n"
            );
        
        add( "stop_repeat" , "delilah",
             "Stop a repeat-command or all of them ( see help repeat for more info)",
             "stop_repeat [process_id]\n"
             "            [process_id] optional id to be stoped\n"
            );
         */
        
        
        add( "ps" , "delilah" ,   
            "Show information about delilah processes",
            "ps <-clear> <id> \n"
            "\n"
            "      -clear    removes finished or error processes\n"
            "      id        It gets more help about a particular process\n" 
            );                
        
        add( "set" , "delilah" ,
            "Set environment variable in delilah",
            "set <var> <value>"
            );
        
        add( "unset" , "delilah" ,
            "Remove an environment variable",
            "unset <var>");
        
        add( "set_mode"  , "delilah",
            "Select delilah working mode: normal, database, logs",
            "set_mode normal/database/logs"
            );
        
        
        add("ls_local", "local", 
            "Show a list of current directory with relevant information about local data-sets");

        
        add("rm_local", "local",
            "Remove a local file or directory (and all its contents)"); 

        add("history", "local",
            "Show the history of the last commands",
            "history [num_items]   Optional parameter to limit the number of history items to show "); 
        
        
        // DATA MANIPULATION
        // ------------------------------------------------------------------
        
        add( "ls" , "data" , "Show a list of all data queues in the system",
			 "ls [-rates] [-blocks] [-properties] [-group group_field]\n"
			 "      -rates        Information about total size and current rate\n"
			 "      -blocks       Detailes information about blocks\n"
			 "      -properties   Get properties assigned to queues\n"
			 "      -group X      Group results by a particular field"
		   );
        
        add( "rm" , "data" , 
			 "Remove a queue",
			 "rm <queue>"
		   );
                
        add( "set_queue_property" , "data" ,
            "Specify the value of property <property> for queue <queue>",
            "set_queue_property <queue> <property> <value>"
            );
        
        add( "unset_queue_property" , "data" ,
            "Remove a previously defined property property for a queue",
            "unset_queue_property [queue] [property]"
            );
        
        add( "show_stream_block" ,"data" , "Show data activity in a particular stream block",
            "It shows input and output queues, state queues, stream operations and internal states included in inner stream blocks\n"
            "show_stream_block path [-rates]\n"
            );
        
        add( "ls_blocks" , "data" , "Show the list of data blocks managed in a SAMSON cluster. This is a debug tool",
            "ls_blocks" );
        
        add( "ls_buffers" , "data" , "Show the list of data buffers managed in a SAMSON cluster. This is a debug tool",
            "ls_buffers" );

        
        // MODULES
        // ------------------------------------------------------------------
        
        
        add( "ls_modules" , "modules",
            "Show a list of modules installed in SAMSON node workers",
            "ls_module [module_name_pattern]. i.e ls_modules web*\n");
        
        add(  "ls_operations","modules", 
            "Shows a list of available operations",
            "ls_operations [operation_name_pattern]. i.e ls_operations web*");
        
        add( "ls_datas" , "modules" ,
            "Shows a list of available data-types.",
            "ls_datas [data_name_pattern]. i.e ls_datas web*"
            );
        
        add( "reload_modules" , "modules", 
            "Reload modules in all workers");
        
        add( "push_module" , "modules", 
            "Push a module to the cluster. The module has to be compatible with hardware architecture",
            "push_module <module_file>"
            );
        
        // STREAM
        // ------------------------------------------------------------------
        
        
        add( "ls_stream_operations"  , "stream" , 
            "Show a list of stream operations defined( added with add_stream_operation)",
            "ls_stream_operations [-in] [-out] [-running] [-properties]\n"
            "      -in          Information about data accepted to these operations\n"
            "      -out         Information about data emitted\n"
            "      -running     Currently running operations and status\n"
            "      -properties  Show properties associated to each stream operation\n"
            );
        
        add( "add_stream_operation" , "stream"    ,   
            "Add an operation to automatically process data from input queues to output queues",
            "add_stream_operation name operation input-queues  output-queues <-forward>n"
            "    -forward      Option allows to schedule reduce operations without state. Joint against a constant queue");
        
        add( "rm_stream_operation" , "stream"  ,  
            "Remove a previously defined operation with add_stream_operation",
            "rm_stream_operation name <-f>"
            "      -f      Option avoids complaints when the operation does not exist");
        
        add( "run" , "stream" ,
            "Run a particular operation over queues manually",
            "run <op_name> [queues...] [-clear_inputs]\n"
            "\n"
            "           <op_name>        : Name of the operation. See 'help ls_operations' for more info\n"
            "           [queues]         : Name of the queues involved in this operation (inputs and outputs)\n"
            "           [-clear_inputs]  : Flag used to remove content from input queues when running this operation\n"
            );

        add( "cancel_stream_operation" , "stream" ,
            "Cancel a particular operation",
            "cancel_stream_operation <op_id>\n"
            "\n"
            "           <op_name>        : Identifier of the operation. Usually something like XXXXX_XXX\n"
            );
        
        
        add( "set_stream_operation_property"    , "stream" ,  
            "Set value of an enviroment property associated to a stream operation ( see add_stream_operation )",
            "set_stream_operation_property stream_operation_name variable_name value");
        
        add( "unset_stream_operation_property"    , "stream" ,  
            "Unset value of an enviroment property associated to a stream operation ( see add_stream_operation )",
            "unset_stream_operation_property stream_operation_name variable_name");
        
        
        add( "add_queue_connection"    , "stream" ,  
            "Connect a queue to a set of queues. Data will be automatically redirected to target queues",
            "add_queue_connection source_queue target_queue_1 target_queue_2 ... target_queue_N ");

        add( "rm_queue_connection"    , "stream" ,  
            "Remove a queue connected stablished with add_queue_connection",
            "rm_queue_connection source_queue target_queue");
        
        add( "ls_queues_connections"    , "stream" ,  
            "Show queue_connections defined with the add_queue_connection command");
        
        add( "ps_stream" , "stream" ,
            "Get a list of current stream tasks currently running in all workers"
            );
        
        
        add( "ls_workers" , "stream" ,
            "Get a list of current workers with current memory/ disk / process status",
            "ls_workers [-disk] [-engine]\n"
            "           [-disk]   Show more information about disk activity in each worker "
            "           [-engine] Show more information about engine activity in each worker "
            );
        
        add( "init_stream" , "stream" ,
            "Execute a initialization script to setup some automatic stream operations",
            "init_stream [prefix] <script_name>\n"
            "          [prefix]         It is used to name operations and queues\n"
            "          <script_name>    Name of the script (i.e. module.script)\n"
            );
        
        
        add( "ps_workers"  , "stream" , 
            "Show a list of commands being executed in each node of the cluster",
            "ps_workers <command pattern>\n"
            );

        
        add( "defrag"  , "stream" , 
            "Defrag content of a particular queue",
            "defrag <queue> <destination_queue>\n"
            );
        
        
        // PUSH&POP
        // ------------------------------------------------------------------
        
        add( "push" , "push&pop" ,
            "Push content of a list of local file/directories to a queue",
            "push file1/dir1 file2/dir2 file3/dir3 ... fileN/dirN <queue>");
        
        add( "pop" , "push&pop"  ,
            "Pop content of a queue to a local directory. Also working for binary queues. Use samsonCat to check content",
            "pop  <queue> <local_file_or_dir> [-force] [-show]");
        
        
        add( "connect_to_queue" , "push&pop"  
            , "Connect to a  queue to receive live data from SAMSON."
            , "connect_to_queue queue" );
        
        add( "disconnect_from_queue" , "push&pop" ,
            "Disconnects from a particular queue to not receive live data from SAMSON",
            "disconnect_from_queue queue"
            );
        
        add( "ls_pop_connections" , "push&pop"  
            , "Show a list of connections to receive live data from SAMSON."
            , "ls_pop_connections" );
        
        
        add("ls_local_queues", "push&pop", 
            "Show a list of local queues ( current directory ).");
        
        add( "show_local_queue" , "push&pop"  ,
            "Show contents of a queue downloaded using pop. Modules should be installed locally",
            "show_local_queue <local_dir> [-header] [-limit X]"
            "          <local_dir>    Name of local directory where queue has been saved\n"
            "          [-header]      Show only headers\n"
            "          [-limit X]     Limit the number of key-values to show ( 10 by default )\n"
            );
        
        add( "push_queue" , "push&pop", 
            "Push content of a queue to another queue/s",
            "push_queue  <from_queue> <to_queue>"
            );
        
        
        // CLUSTER
        // ------------------------------------------------------------------
        
        add( "ls_connections" , "management" ,
            "Show status of all connections in the cluster");
        
        add( "cluster" , "management" ,
            "Command for cluster related operation",
            "cluster info/connect/add/remove/reset/get_my_id [...options...]\n"
            "\n"
            "           info                                         : Show current cluster definition and connection status\n"
            "           connections                                  : Show more information about current connections\n"
            "           pending                                      : Show pending packets for unconnected workers\n"
            "           connect host[:port] [-user X] [-password X]  : Connect to another SAMSON cluster instance.\n"
            "           add host[:port]                              : Add a node to this cluster. A samsond clear instance should be running there\n"
            "           remove id                                    : Remove one of the involved workers. The worker id should be provided ( see cluster info )\n"
            "           get_my_id                                    : Get this delilah identifier ( see ls_connections )\n"
            );
        
        
        
        // Extra description
        add_description("connect_to_queue", "Received data will be stored in a local directory called stream_out_<queue>");
        add_description("ls_local_queues", "These queues have been typically being downloaded with command pop.");

        add_description( "cancel_stream_operation" ,"This command cancels both"
                        "\n\toperations executed manually ( see run )"
                        "\n\toperations executed automatically  ( see add_stream_operation )");
        
    }
    
    void DelilahCommandCatalogue::add(
                                      std::string name 
                                      , std::string category 
                                      , std::string short_description 
                                      , std::string usage )
    {
        commands.push_back( DelilahCommand(name , category , short_description , usage ) );            
    }
    
    void DelilahCommandCatalogue::autoComplete( au::ConsoleAutoComplete* info )
    {
        for ( size_t i = 0 ; i < commands.size() ; i++ )
            info->add(commands[i].name);
    }
    
    void DelilahCommandCatalogue::add_description( std::string name , std::string description )
    {
        for ( size_t i = 0 ; i < commands.size() ; i++ )
            if( commands[i].name == name )
            {
                commands[i].description = description;
                return;
            }
        
        DelilahCommand command( name , "general" );
        command.description = description;
        commands.push_back(command);
    }
    
    
    std::string DelilahCommandCatalogue::getCommandsTable( std::string category )
    {
        au::StringVector columns = au::StringVector("Command"  , "Category,left" , "Description,left" );
        au::tables::Table table( columns );
        
        for( size_t i = 0 ; i < commands.size() ; i++ )
        {
            if( category == "" || (commands[i].category == category ) )
                table.addRow( au::StringVector( commands[i].name , commands[i].category ,  commands[i].short_description ) );
        }

        // Print the table
        
        if( category == "" )
            table.setTitle( "All commands" );
        else
            table.setTitle( au::str( "Commands of category %s" , category.c_str() ) );
        
        return table.str();
    }
    
    
    std::string DelilahCommandCatalogue::getHelpForCommand( std::string name )
    {
        for ( size_t i = 0 ; i <  commands.size() ; i++ )
            if( commands[i].name == name )
            {
                std::ostringstream output;
                
                output << au::lineInConsole('-') << "\n";
                output << au::str( au::purple , " COMMAND %s       ( %s )" , name.c_str() , commands[i].category.c_str() );
                output << "\n";
                output << au::lineInConsole('-') << "\n";
                output << "\n";
                output << au::str( au::purple , "DESCRIPTION:  " ) << commands[i].short_description << "\n";
                if( commands[i].description != "" )
                {
                    output << au::str_indent( commands[i].description , 14 );
                    output << "\n";
                }
                
                
                if( commands[i].usage != "" )
                {
                    output << au::str( au::purple , "USAGE: ") << "\n";
                    output << au::str_indent( commands[i].usage , 14 );
                    output << "\n";
                }
                
                output << au::lineInConsole('-') << "\n";
                
                return output.str();
            }
        
        
        // Check if it is a category...
        if( isValidCategory( name ) )
            return getCommandsTable( name );
        
        // Show list of categories
        if( name == "categories" )
        {
            au::StringVector cats = getCategories();
            std::ostringstream output;
            output << "\nCategories:\n\n";
            for ( size_t i = 0 ; i < cats.size() ; i++ )
                output << "\tCategory " << cats[i] << "\n";
            return output.str();
        }
        
        return au::str( "Unknown command %s\n" , name.c_str() );
    }
    
    
    au::StringVector DelilahCommandCatalogue::getCategories()
    {
        au::StringVector categories;
        for ( size_t i = 0 ; i < commands.size() ; i++ )
            categories.push_back( commands[i].category );
        
        categories.unique();
        return categories;
    }
    
    
    bool DelilahCommandCatalogue::isValidCommand( std::string command )
    {
        for ( size_t i = 0 ; i < commands.size() ; i++ )
            if( commands[i].name == command )
                return true;
        return  false;
    }

    bool DelilahCommandCatalogue::isValidCategory( std::string category )
    {
        for ( size_t i = 0 ; i < commands.size() ; i++ )
            if( commands[i].category == category )
                return true;
        return  false;
    }
    

}
