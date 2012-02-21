#ifndef _H_DelilahConsole
#define _H_DelilahConsole

/* ****************************************************************************
*
* FILE                     DelilahConsole.h
*
* DESCRIPTION			   Console terminal for delilah
*
* Portions Copyright (c) 1997 The NetBSD Foundation, Inc. All rights reserved
*/
#include <cstdlib>				// atexit

#include <sstream>                  // std::ostringstream
#include <time.h>
#include <fcntl.h>
#include <stdio.h>
#include <errno.h>
#include <algorithm>

#include "logMsg/logMsg.h"				

#include "au/TokenTaker.h"                  // au::TokenTake
#include "au/Console.h"                     // au::Console
#include "au/ConsoleAutoComplete.h"

#include "tables/Select.h"
#include "tables/Table.h"

#include "DelilahClient.h"                  // ss:DelilahClient

#include "samson/delilah/Delilah.h"			// samson::Delilah

namespace samson {
	
    class DelilahCommand
    {
        
    public:
        
        std::string name;
        std::string category;
        
        std::string short_description;
        
        std::string description;
        std::string usage;
        
        DelilahCommand(
                       std::string _name , 
                       std::string _category="general" , 
                       std::string _short_description = "",
                       std::string _usage = "" 
                       )
        {
            name = _name;
            category = _category;
            short_description = _short_description;
            usage = _usage;
        }
        
    };
    
    class DelilahCommandCatalogue
    {
    public:
        
        std::vector<DelilahCommand> commands;
        
        DelilahCommandCatalogue()
        {
            
            // DELILAH
            // ------------------------------------------------------------------
            
            add( "help","delilah","Get some help about SAMSON platform",
                "help                        Get general help\n"
                "help all [-category cat]    Get all available commands ( optional category )\n" 
                "help command                Get help for a particular command\n"
                );
            
            add( "trace" , "delilah", 
                "Activate or disactivate showing traces from running operations",
                "trace <on> <off>"
                );

            add( "send_trace" , "delilah", 
                "Send a trace to all conneceted delilah using a random worker as a sender\n",
                "send_trace [-worker X] \"Message to be sent\""
                "      -worker X     Use only this worker as broadcaster\n"
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
            
            add( "cp_queue" , "data", 
                "Copy contents of queue <from_queue> to queue <to_queue>",
                "cp_queue  <from_queue> <to_queue>"
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
                "ls_module <name>\n");

            add(  "ls_operations","modules", 
                "Shows a list of available operations",
                "ls_operations <op_name>");
            
            add( "ls_datas" , "modules" ,
                "Shows a list of available data-types."
                "ls_operations <data_name>"
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

                
 
            add( "set_stream_operation_property"    , "stream" ,  
                "Set value of an enviroment property associated to a stream operation ( defined with add_stream_operation )",
                "set_stream_operation_property stream_operation_name variable_name value");

            add( "unset_stream_operation_property"    , "stream" ,  
                "Unset value of an enviroment property associated to a stream operation ( defined with add_stream_operation )",
                "unset_stream_operation_property stream_operation_name variable_name");

            add( "ps_stream" , "stream" ,
                "Get a list of current stream tasks currently running in all workers"
                );

 
            add( "init_stream" , "stream" ,
                "Execute a initialization script to setup some automatic stream operations",
                "init_stream [prefix] <script_name>\n"
                "          [prefix]         It is used to name operations and queues\n"
                "          <script_name>    Name of the script (i.e. module.script)\n"
                );

            add( "run_stream_operation" , "stream" ,
                "Run a particular operation over queues",
                "run_stream_operation <op_name> [queues...] [-clear_inputs]\n"
                "\n"
                "           <op_name>        : Name of the operation. See 'help ls_operations' for more info\n"
                "           [queues]         : Name of the queues involved in this operation (inputs and outputs)\n"
                "           [-clear_inputs]  : Flag used to remove content from input queues when running this operation\n"
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
            
            
            // CLUSTER
            // ------------------------------------------------------------------

            add( "ls_connections" , "cluster" ,
                "Show status of all connections in the cluster");
            
            add( "cluster" , "cluster" ,
                "Command for cluster related operation",
                "cluster info/connect/add/remove/reset [...options...]\n"
                "\n"
                "           info                 : Show current cluster definition and connection status\n"
                "           connections          : Show more information about current connections\n"
                "           connect host [port]  : Connect to another SAMSON cluster instance.\n"
                "           add host [port]      : Add a node to this cluster. A samsond clear instance should be running there \n"
                "           remove id            : Remove one of the involved workers. The worker id should be provided ( see cluster info )\n"
                );
            
            
            
/* 
 
 { "ls_block_manager"        ,   "Get information about the block-manager for each worker\n" 
 "Type 'help stream_processing' for more information\n"
 },
 
 { "ls_blocks"               ,   "Get a complete list of blocks managed at each worker"
 },
 
 
 { "ls_operation_rates"      ,   "Get a list of statistics about operations in the platform\n"},
 
 
 { "ls_stream_activity"      ,   "Show a list of the last activity logs about automatic stream processing\n\n"
 "Type 'help stream_processing' for more information\n"
 },
 
 */
            
            
            add_description("ls_modules" ,
                            "Usage: ls_modules [name]\n"
                            "\t[name] Optional name (or first part of the name) of a module to filter output\n");
            
            
        }
        
        void add( std::string name , std::string category , std::string short_description = "" , std::string usage = "" )
        {
            commands.push_back( DelilahCommand(name , category , short_description , usage ) );            
        }

        void autoComplete( au::ConsoleAutoComplete* info )
        {
            for ( size_t i = 0 ; i < commands.size() ; i++ )
                info->add(commands[i].name);
        }
        
        void add_description( std::string name , std::string description )
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
        
        std::string getCommandsTable( std::string category="" )
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
        
        
        std::string getHelpForCommand( std::string name )
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
                    output << au::indent( commands[i].description );
                    output << "\n";
                    output << au::lineInConsole('-') << "\n";
                    
                    return output.str();
                }
            
            return au::str( au::red , "Unknown command %s\n" , name.c_str() );
        }
        
        
        au::StringVector getCategories()
        {
            au::StringVector categories;
            for ( size_t i = 0 ; i < commands.size() ; i++ )
                categories.push_back( commands[i].category );
            
            categories.unique();
            return categories;
        }
        
    };    
    
	/**
	 Main class for the DelilahConsole program
	 */
	
	class DelilahConsole : public au::Console, public Delilah
	{
		
        std::string commandFileName;
        bool database_mode; // Flag to indicate that we are running commands over table collection
        
        
        DelilahCommandCatalogue delilah_command_catalogue;
        
	public:
		
        
		DelilahConsole( NetworkInterface *network );		
		~DelilahConsole();
				
		// Console funciton
		// --------------------------------------------------------
		
		
        // Main run command
        void run();
        
        // Set the command-file
        void setCommandfileName( std::string _commandFileName)
        {
            commandFileName = _commandFileName;
        }
                
		// Eval a command from the command line
        virtual std::string getPrompt();
		virtual void evalCommand( std::string command );
        virtual void autoComplete( au::ConsoleAutoComplete* info );

        virtual void process_escape_sequence( std::string sequence )
        {
            if( sequence == "samson" )
            {
                writeWarningOnConsole("SAMSON's cool ;)");
            }

            if( sequence == "d" )
            {
                database_mode = !database_mode;
                refresh(); // refresh console
            }
                
        }
        
        
		// Run asynch command and returns the internal operation in delilah
		size_t runAsyncCommand( std::string command );

		// Functions overloaded from Delilah
		// --------------------------------------------------------
		
        void delilahComponentFinishNotification( DelilahComponent *component);
        void delilahComponentStartNotification( DelilahComponent *component);
        
		// Function to process messages from network elements not handled by Delila class
		int _receive( Packet* packet );		

		// Notify that an operation hash finish
		virtual void notifyFinishOperation( size_t id )
		{
			std::ostringstream output;
			output << "Finished local delilah process with : " << id ;
			writeWarningOnConsole( output.str() );
		}
		
		// Show a message on screen
		void showMessage( std::string message)
		{
			writeOnConsole( au::strToConsole( message ) );
		}
		void showWarningMessage( std::string message)
        {
			writeWarningOnConsole( au::strToConsole( message ) );
        }
        
		void showErrorMessage( std::string message)
        {
			writeErrorOnConsole( au::strToConsole( message ) );
        };

		
		virtual void showTrace( std::string message)
		{
			if( trace_on )
				writeWarningOnConsole( message );
		}
        
        
        void run_repeat_command( std::string command )
        {
            LM_TODO(("Check what type of messages...."));
            runAsyncCommand(command);
        }

	};

}


#endif
