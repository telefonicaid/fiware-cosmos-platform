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
#include "samson/delilah/DelilahUtils.h"    // getXXXInfo()

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
                       std::string _description = "" 
                       )
        {
            name = _name;
            category = _category;
            short_description = _short_description;
            description = _description;
        }
        
    };
    
    class DelilahCommandCatalogue
    {
    public:
        
        std::vector<DelilahCommand> commands;
        
        DelilahCommandCatalogue()
        {
            
            // GENERAL
            // ------------------------------------------------------------------
            
            add( "help","general","Get some help about SAMSON platform",
                "help                        Get general help\n"
                "help all [-category cat]    Get all available commands ( optional category )\n" 
                "help command                Get help for a particular command\n"
                );

            // OPERATIONS
            // ------------------------------------------------------------------
            
            add("ls_modules", "operations",
                "Show a list of available modules",
                "ls_modules name: Show modules that begin with 'name'");

            add("ls_operations", "operations",
                "Show a list of available modules",
                "ls_operations name: Show operations that begin with 'name'\n"
                "Option -v show more information about input-output format "
                "Option -first to show only one result"
                );
            
            // DATA MANIPULATION
            // ------------------------------------------------------------------
            
            add( "ls_queues" , "data" , "Show a list of all data queues in the system",
                "Option -v -vv        Get more information from queues\n"
                "Option -w            Get information for each worker\n"
                );

            // LOCAL
            // ------------------------------------------------------------------
            
            add("ls_local", "local", 
                "Show a list of current directory with relevant information about local data-sets",
                "It is very useful to list local dirs containing local data-sets\n");
            
            add("rm_local", "local",
                "Remove a local directory and all its contents"); 
            
            
/*
 
 { "ls_modules"              ,   "Usage: ls_modules [name]\n\n"
 "Show a list of modules installed at controller, workers and delilah\n" 
 "Inputs:\n"
 "\t[name] Optional name (or first part of the name) of a module to filter output\n"
 } ,                
 { "rm"                      ,   "Usage: rm <set>\n\n"
 "Remove a key-value set (batch processing)" 
 },              
 { "mv"                      ,   "Usage mv <set> <new_set>\n\n"
 "Change the name of a particular key-value set (batch processing)"
 },
 { "trace"                      ,    "Usage trace <on> <off>\n\n"
 "Activate or disactivate showing traces from running operations"
 },
 { "clear"                   ,   "Usage: clear <set>\n\n"
 "Clear the content of a particular key-value set\n"
 },
 { "repeat"                   ,   "Usage: repeat <command>. Stop them using stop_repeat\n\n"
 "Repeat a command continuously\n"
 },
 { "stop_repeat"                   ,   "Stop all repeat-commands. See repeat command\n\n"
 "Clear the content of a particular key-value set\n"
 },
 { "ls_processes"            ,   "Shows a list with all the processes (batch and stream) running at workers"},
 
 { "ls_operations"           ,   "Usage: ls_operations [op_name]\n\n"
 "Shows a list of available operations.\n"
 "Inputs:\n"
 "\t[op_name] : Name or first letters of an operation\n"
 },
 { "ls_datas"                ,   "Usage: ls_datas [data_name]\n\n"
 "Shows a list of available data-types.\n"
 "Inputs:\n"
 "\t[data_name] : Name or first letters of a data-type\n"
 },
 { "ps"                      ,   "Usage: ps [-clear] [id]\n\n"
 "Show information about delilah processes.\n"
 "Inputs:\n"
 "\t[-clear] removes finished or error processes\n"
 "\t[id] Identifier of a process. It gets more help about this particular process" 
 } ,                
 
 { "ps_network"              ,   "Get information about network connections in the SAMSON cluster"},
 { "reload_modules"          ,   "Reload modules at controller and workers"},
 
 
 { "add"                     ,   "Usage:add <set> <key-format> <value-format> [-txt] [-f]\n\n"
 "Add a key-value or txt data set (batch processing)"
 "Inputs:\n"
 "\t<set>\tName of the new set\n"
 "\t<key-format>    DataType used for key\n"
 "\t<value-format>  DataType used for value\n"
 "\t[-txt]          It creates a txt data-set\n"
 "\t[-f]            Flag to suppress the error if <set> already exists (with the same key-value types). If different key-value types, still error"
 },
 
 { "sets"                    ,   "Environment variables are used to set some local properties at delilah that will be sent along with "
 "all the command we sent to SAMSON platform. Use set and unset to define and remove these environment "
 "variable. Note that third party software executed at SAMSON can use these values"
 },
 { "set"                     ,   "Usage: set <var> <value>\n\n"
 "Set environment variable <set> to value <value>\n" 
 "Type 'help sets' for more information about environment variables\n"
 },
 { "unset"                   ,   "Usage: unset <var>\n\n"
 "Remove an environment variable\n"
 "Inputs:\n"
 "\t<var> Name of the environment variable we want to remove.\n"
 "Type 'help sets' for more information about environment variables\n"
 },
 
 
 { "ps_jobs"                 ,   "Get a list of running or finished (batch processing) jobs in the platform" },
 { "ps_tasks"                ,   "Get a list of running batch processing tasks on controller and workers" },
 { "clear_jobs"              ,   "Clear finished or error marked jobs in the platform" },
 { "kill"                    ,   "Usage: kill <job>\n\n"
 "Kill job <job> and all its sub-tasks" 
 },
 
 { "upload_data"             ,   "To upload a txt file to samson platform, follow these steps:\n"
 " * Create a txt data set with add command : add my_set -txt\n"
 " * Upload the content with upload command : upload my_local_file my_set\n\n"
 "Once data is uploaded to the platform, several operations can be applied for processing\n"
 "Try 'txt.parser_words my_set my_words -c' to transform this txt set into a binary one containing words\n\n"
 "Any generated data-set (txt or binary) can be downloaded to a local directory with download command\n"
 "Try download my_words local_my_words\n\n"
 "Finally, use samsonCat utility to visualize the content of these downloaded files.\n\n"
 "Type 'help add' , 'help upload' and 'help download' for help on individual commands\n"
 },
 { "upload"                  ,   "Usage: upload  <local_file_ir_directory> <set>\n\n"
 "Upload txt files to the platform (batch processing)" 
 "Inputs:\n"
 "\t<local_file_ir_directory>    Local txt file or directory containing txt files\n"
 "\t<set>                        Name of the data set in SAMSON platform (create it with add -txt <set>)"
 "Type 'help upload_data for more information about upload and download process\n"
 },
 { "download"                ,   "Usage: download  <set> <local_directory_name> [-force]\n\n"
 "Download the dataset from the platform to local directory\n" 
 "Inputs:\n"
 "\t<local_directory_name>    Local name of the directory where files will be downloaded\n"
 "\t<set>                     Name of the data set in SAMSON platform (create it with add -txt <set>)\n"
 "\t[-force]                  Remove local directory if it exists\n"
 "Note: It is not necessary to create the directory. Delilah will do it for you\n",
 },
 
 { "push"                    ,   "Usage: push <local_file_or_dir> <queue>\n\n"
 "Push content of a local file/directory to a queue"},
 { "pop"                     ,   "Usage: pop <queue> <local_dir>\n\n"
 "Pop content of a queue to a local directory. Also working for binary queues. Use samsonCat to check content"},
 
 { "add_stream_operation"    ,   "Usage: add_stream_operation <name> <operation> <input .. output queues> [-forward] \n\n"
 "Add an operation to automatically process data from input queues to output queues\n"
 "'-forward' option allows to schedule reduce operations without state\n\n"},
 
 { "rm_stream_operation"     ,   "Usage: rm_stream_operation <name> [-f]\n\n"
 "Remove a previously introduced operation with add_stream_operation\n"
 "'-f' option avoids complaints when the operation does not exist\n\n"},
 
 { "set_stream_operation_property"     , "Usage: set_stream_operation_property <name> <variable> <value>\n\n"},
 
 { "ls_stream_operations"            , "Usage: ls_stream_operations [-v] [-vv]\n"
 "Show a list of stream operations. Use '-v' and '-vv' options for more verbose output\n"},
 
 { "rm_queue"                ,   "Usage: rm_queue <queue>\n\n"
 "Remove queue <queue>\n" 
 "Type 'help queues' for more information\n"
 },
 
 
 { "queues"              ,   "SAMSON platform is organized in queues for stream processing.\n" 
 "A queue is basically a box where blocks of data are accumulated\n\n"
 "The simplest way to create a queue is pushing some local txt files to it. Try this...\n"
 " * push <local_file> <queue_name>\n"
 " * ls_queues \n"
 "\n"
 "Once data is accumulated in a particular queue, we can process using run_stream_operation or add_stream_operation. Try this...\n"
 " * run_stream_operation txt.parser_words <input_queue> <output_queue> -clear_inputs\n"
 "A new queue is generated with the contents of the operation's output\n"
 "Finally we can download contents of a queue to a local directory with...\n"
 " * pop <queue> <local_directory>\n"
 "Use samsonCat to visualise contents of downloaded files\n"
 
 },
 { "stream_processing"   , "Information about stream_processing coming soon..."
 },
 { "batch_processing"    , "Information about batch_processing coming soon..."
 },
 
 { "ls_queues"               ,   "Usage: ls_queues [queue] [-v][-vv]\n\n"
 "Show a list of current queues in all the workers\n" 
 "Inputs:\n"
 "\t[queue] : Name or first letters of queue to filter the output of this command\n"
 "\t[-v]    : Verbose mode\n"
 "\t[-vv]   : More verbose mode"
 "\n"
 "Type 'help queues' for more information\n"
 
 },
 
 { "ps_stream"               , "Get a list of current stream tasks. Type 'help stream_processing' for more help."
 },
 
 { "ls_engines"             , "Show a status information of the engine (lower level system in SAMSON) in all workers and controller"},
 
 { "init_stream"    ,   "Usage: init_stream [prefix] <script_name>\n\nInit stream operations from a script\n"
 "\t[prefix]       :if defined, it is used to name operations and queues\n"
 "\t<script_name>  :the name of the script (i.e. module.script), where stream operations are set with add_stream_operation\n"
 },
 
 { "run_stream_operation"    ,   "Usage: run_stream_operation <op_name> [queues...] [-clear_inputs]\n\n"
 "Run a particular operation over queues.\n"
 "Inputs:\n"
 "\t<op_name>        : Name of the operation. See 'help ls_operations' for more info\n"
 "\t[queues]         : Name of the queues involved in this operation (inputs and outputs)\n"
 "\t[-clear_inputs]  : Flag used to remove content from input queues when running this operation\n"
 "Type 'help stream_processing' for more information\n"
 },
 
 
 { "ls_block_manager"        ,   "Get information about the block-manager for each worker\n" 
 "Type 'help stream_processing' for more information\n"
 },
 
 { "ls_blocks"               ,   "Get a complete list of blocks managed at each worker"
 },
 
 { "cp_queue"                ,   "Usage: cp_queue  <from_queue> <to_queue>\n\n"
 "Copy contents of queue <from_queue> to queue <to_queue>\n"
 "Type 'help queues' for more information\n"
 },
 
 { "ls_operation_rates"      ,   "Get a list of statistics about operations in the platform\n"},
 { "set_queue_property"      ,   "Usage: set_queue_property <queue> <property> <value>\n\n"
 "Specify the value of property <property> for queue <queue>\n"
 "Type 'help queues' for more information\n"
 },
 
 { "ls_stream_activity"      ,   "Show a list of the last activity logs about automatic stream processing\n\n"
 "Type 'help stream_processing' for more information\n"
 },
 
 { "connect_to_queue"        , "Connect to a particular queue to receive live data from SAMSON"},
 { "disconnect_from_queue"   , "Disconnects from a particular queue to not receive live data from SAMSON"},
 
 { "set_database_mode"       , "set_database_mode on/off    Activate or deactivate debug database mode"},
 
 
 */
            
            
            add_description("ls_modules" ,
                            "Usage: ls_modules [name]\n"
                            "\t[name] Optional name (or first part of the name) of a module to filter output\n");
            
            
        }
        
        void add( std::string name , std::string category , std::string short_description="" , std::string description = "" )
        {
            commands.push_back( DelilahCommand(name , category , short_description , description ) );            
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
            
            return au::str( au::red , "Unknown command %s" , name.c_str() );
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
		int _receive(int fromId, Message::MessageCode msgCode, Packet* packet);		

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
			writeOnConsole( message );
		}
		void showWarningMessage( std::string message)
        {
			writeWarningOnConsole( message );
        }
        
		void showErrorMessage( std::string message)
        {
			writeErrorOnConsole( message );
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
