

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
        
        add( "alerts" , "delilah", 
            "Activate or disactivate showing alerts from SAMSON platform on console",
            "alerts <on> <off>"
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
        
        
        add( "send_alert" , "delilah", 
            "Send a trace to all conneceted delilah using a random worker as a sender\n",
            "send_trace [-worker X] [-error] [-warning] \"Message to be sent\""
            "      -worker X     Use only this worker as broadcaster\n"
            "      -error        Mark this trace as an error for correct visualitzation\n"
            "      -warning      Mark this trace as a warning for correct visualitzation\n"
            "      message       Use only this worker as broadcaster\n"
            );
        
        add( "repeat" , "delilah",
            "Repeat a command continuously",
            "repeat <command>. Stop them using stop_repeat\n\n"
            );
        
        add( "stop_repeat" , "delilah",
            "Stop all repeat-commands. See repeat command"
            );
        
        
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
        
        add( "set_database_mode"  , "delilah" , 
            "Activate or deactivate debug database mode",
            "set_database_mode on/off"
            );
        
        
        add("ls_local", "local", 
            "Show a list of current directory with relevant information about local data-sets");
        
        add("rm_local", "local",
            "Remove a local directory and all its contents"); 
        
        add( "ls_engines"  , "stream",
            "Show a status information of the engine (lower level system in SAMSON) in all workers ");
        
        // DATA MANIPULATION
        // ------------------------------------------------------------------
        
        add( "ls_queues" , "data" , "Show a list of all data queues in the system",
            "ls_queues [-rates] [-blocks] [-properties] [-group group_field]"
            "      -rates        Information about total size and current rate\n"
            "      -blocks       Detailes information about blocks\n"
            "      -properties   Get properties assigned to queues\n"
            "      -group X      Group results by a particular field"
            );
        
        add( "rm_queue" , "data" , 
            "Remove a queue" 
            "Usage: rm_queue queue"
            );
                
        add( "set_queue_property" , "data" ,
            "Specify the value of property <property> for queue <queue>",
            "set_queue_property [queue] [property] [value]"
            );
        
        add( "unset_queue_property" , "data" ,
            "Remove a previously defined property property for a queue",
            "unset_queue_property [queue] [property]"
            );
        
        add( "show_stream_block" ,"data" , "Show data activity in a particular stream block",
            "show_stream_block path\n"
            );
        
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
        
        
        // STREAM
        // ------------------------------------------------------------------
        
        
        add( "ls_stream_operations"  , "stream" , 
            "Show a list of stream operations defined( added with add_stream_operation)",
            "ls_stream_operations [-in] [-out] [-running]\n"
            "      -in          Information about data accepted to these operations\n"
            "      -out         Information about data emmitted\n"
            "      -running     Currently running operations and status\n"
            "      -properties  Show properties assiciated to each stream operation\n"
            );
        
        add( "add_stream_operation" , "stream"    ,   
            "Add an operation to automatically process data from input queues to output queues",
            "add_stream_operation name operation input-queues  output-queues <-forward>n"
            "    -forward      Option allows to schedule reduce operations without state. Joint against a constant queue");
        
        add( "rm_stream_operation" , "stream"  ,  
            "Remove a previously defined operation with add_stream_operation",
            "rm_stream_operation name <-f>"
            "      -f      Option avoids complaints when the operation does not exist");
        
        add( "run_stream_operation" , "stream" ,
            "Run a particular operation over queues manually",
            "run_stream_operation <op_name> [queues...] [-clear_inputs]\n"
            "\n"
            "           <op_name>        : Name of the operation. See 'help ls_operations' for more info\n"
            "           [queues]         : Name of the queues involved in this operation (inputs and outputs)\n"
            "           [-clear_inputs]  : Flag used to remove content from input queues when running this operation\n"
            );
        
        
        add( "set_stream_operation_property"    , "stream" ,  
            "Set value of an enviroment property associated to a stream operation ( defined with add_stream_operation )",
            "set_stream_operation_property stream_operation_name variable_name value");
        
        add( "unset_stream_operation_property"    , "stream" ,  
            "Unset value of an enviroment property associated to a stream operation ( defined with add_stream_operation )",
            "unset_stream_operation_property stream_operation_name variable_name");
        
        add( "ps_stream" , "stream" ,
            "Get a list of current stream tasks currently running in all workers"
            );
        
        
        add( "ls_workers" , "stream" ,
            "Get a list of current workers with current memory/ disk / process status"
            );
        
        add( "init_stream" , "stream" ,
            "Execute a initialization script to setup some automatic stream operations",
            "init_stream [prefix] <script_name>\n"
            "          [prefix]         It is used to name operations and queues\n"
            "          <script_name>    Name of the script (i.e. module.script)\n"
            );
        
        
        add( "ls_worker_commands"  , "stream" , 
            "Show a list of commands being executed in each node of the cluster",
            "ls_worker_commands <command pattern>\n"
            );
        
        
        // PUSH&POP
        // ------------------------------------------------------------------
        
        add( "push" , "push&pop" ,
            "Push content of a local file/directory to a queue",
            "push <local_file_or_dir> <queue>");
        
        add( "pop" , "push&pop"  ,
            "Pop content of a queue to a local directory. Also working for binary queues. Use samsonCat to check content",
            "push <local_file_or_dir> <queue>");
        
        
        add( "connect_to_queue" , "push&pop"  
            , "Connect to a particular queue to receive live data from SAMSON"
            , "connect_to_queue queue" );
        
        add( "disconnect_from_queue" , "push&pop" ,
            "Disconnects from a particular queue to not receive live data from SAMSON",
            "disconnect_from_queue queue"
            );
        
        
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
        
        add( "ls_connections" , "cluster" ,
            "Show status of all connections in the cluster");
        
        add( "cluster" , "cluster" ,
            "Command for cluster related operation",
            "cluster info/connect/add/remove/reset/get_my_id [...options...]\n"
            "\n"
            "           info                                         : Show current cluster definition and connection status\n"
            "           connections                                  : Show more information about current connections\n"
            "           connect host [port] [-user X] [-password X]  : Connect to another SAMSON cluster instance.\n"
            "           add host [port]                              : Add a node to this cluster. A samsond clear instance should be running there \n"
            "           remove id                                    : Remove one of the involved workers. The worker id should be provided ( see cluster info )\n"
            "           get_my_id                                    : Get this delilah identifier ( see ls_connections )\n"
            );
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
        au::StringVector columns = au::StringVector("Command"  , "Description" );
        au::tables::Table *table = new au::tables::Table( columns );
        
        for( size_t i = 0 ; i < commands.size() ; i++ )
        {
            if( category == "" || (commands[i].category == category ) )
                table->addRow(au::StringVector( commands[i].name , commands[i].short_description ));
        }
        
        // Print the table
        au::tables::SelectTableInformation select_table_information;
        select_table_information.addColumn("Command");
        select_table_information.addColumn("Description,left");
        
        if( category == "" )
            select_table_information.title = "All commands";
        else
            select_table_information.title = au::str( "Commands of category %s" , category.c_str() );
        
        std::string output = table->str(&select_table_information);
        delete table;
        return output;
    }
    
    
    std::string DelilahCommandCatalogue::getHelpForCommand( std::string name )
    {
        for ( size_t i = 0 ; i <  commands.size() ; i++ )
            if( commands[i].name == name )
            {
                std::ostringstream output;
                
                output << au::lineInConsole('-') << "\n";
                output << name << "\n";
                output << au::lineInConsole('-') << "\n";
                output << "\n";
                output << "DESCRIPTION:  " << commands[i].short_description << "\n";
                
                if( commands[i].usage != "" )
                    output << "USAGE:        " << commands[i].usage << "\n";
                
                output << "\n";
                output << au::str_indent( commands[i].description );
                output << "\n";
                output << au::lineInConsole('-') << "\n";
                
                return output.str();
            }
        
        return au::str( au::red , "Unknown command %s\n" , name.c_str() );
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

}