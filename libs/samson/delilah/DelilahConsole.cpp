/* ****************************************************************************
 *
 * FILE                     DelilahConsole.h
 *
 * DESCRIPTION			   Console terminal for delilah
 *
 */

#include <iostream>
#include <iomanip>
#include <sys/stat.h>					// stat(.)
#include <dirent.h>						// DIR directory header	


#include "au/CommandLine.h"				// au::CommandLine
#include "au/string.h"					// au::Format
#include "au/Descriptors.h"             // au::Descriptors

#include "engine/MemoryManager.h"                   // samson::MemoryManager

#include "pugi/pugi.h"                  // pugi::Pugi
#include "pugi/pugixml.hpp"             // pugi:...

#include "samson/common/Info.h"                     // samson::Info
#include "samson/common/EnvironmentOperations.h"	// Environment operations (CopyFrom)

#include "samson/network/Packet.h"						// ss:Packet

#include "samson/module/ModulesManager.h"           // samson::ModulesManager
#include "samson/module/samsonVersion.h"            // SAMSON_VERSION

#include "samson/stream/BlockManager.h"             // samson::stream::BlockManager

#include "samson/delilah/SamsonDataSet.h"
#include "samson/delilah/Delilah.h"					// samson::Delailh
#include "samson/delilah/DelilahConsole.h"			// Own interface
#include "UploadDelilahComponent.h"                 // samson::DelilahLoadDataProcess
#include "DownloadDelilahComponent.h"               // samson::DelilahLoadDataProcess
#include "PushDelilahComponent.h"                   // samson::PushDelilahComponent
#include "PushDelilahComponent.h"                   // PushDataComponent


#define DEF1             "TYPE:EXEC/FUNC: TEXT"

namespace samson
{	
	static void consoleFix(void)
	{
		printf("\n");
		write_history(NULL);
		rl_deprep_terminal();
	}
    
	int common_chars( const char* c1 , const char* c2)
    {
        int l = std::min( strlen(c1), strlen(c2));
        
        for ( int i = 0 ; i < l ; i++)
            if( c1[i] != c2[i] )
                return i;
        
        return l;
        
    }
    
    char* strdup_common(const char* c, int len)
    {
        char *d = (char*) malloc( len +1 );
        memcpy(d, c, len);
        d[len] = '\0';
        
        return d;
    }
	
	
	char** readline_completion(const char* text, int start, int end)
	{
		//std::cerr << "Readline completion with \"" << text << "\" S=" << start << " E=" << end <<" \n";
        
        AutoCompletionOptions op;
		
        if( start == 0)
        {
            // Add the main commands since it is the begining of the line
            op.addMainCommands();
            
            // Add all the operations
            op.addOperations();
        }
        else
        {
            // Parse the current introduced line to know how to auto-complete...
            // Note: We are having problems since "rl_line_buffer" is empty :(
            
			au::CommandLine cmdLine;
			cmdLine.parse( rl_line_buffer );
			std::string mainCommand = cmdLine.get_argument(0);
            
			if( ( mainCommand == "clear" ) || ( mainCommand == "rm" ) || ( mainCommand == "cp" ) || ( mainCommand == "mv" ) )
            {
                // Add all the queues
                op.addQueues( );
            }
            else if( mainCommand == "help" )
            {
                op.addMainCommands();
                op.addOperations();
            }
			else 
			{
				// Get the argument position depending of the number of arguments written in the command line
				int argument_pos = cmdLine.get_num_arguments() - 1;
				if ( ( argument_pos > 0 )  && (rl_line_buffer[strlen(rl_line_buffer)-1]!=' '))
					argument_pos--;	// Still in this parameter
				
                op.addQueueForOperation( mainCommand , argument_pos );
            }
            
	    }
        
        return  op.get(text);
        
	}	
	
	void DelilahConsole::evalCommand(std::string command)
	{
		runAsyncCommand(command);
	}
	
    void DelilahConsole::run()
    {
		//LM_M(("atexit(consoleFix)"));
		atexit(consoleFix);
		// rl_prep_terminal(0);
        
        // If command-file is provided
        if ( commandFileName.length() > 0 )
        {
            FILE *f = fopen( commandFileName.c_str() , "r" );
            if( !f )
            {
                LM_E(("Error opening commands file %s", commandFileName.c_str()));
                exit(0);
            }
            
            char line[1024];
            
            //LM_M(("Processing commands file %s", commandFileName ));
            while( fgets(line, sizeof(line), f) )
            {
                // Remove the last return of a string
                while( ( strlen( line ) > 0 ) && ( line[ strlen(line)-1] == '\n') > 0 )
                    line[ strlen(line)-1]= '\0';
                
                //LM_M(("Processing line: %s", line ));
                size_t id = runAsyncCommand( line );
                
                if( id != 0)
                {
                    //LM_M(("Waiting until delilah-component %ul finish", id ));
                    // Wait until this operation is finished
                    while ( isActive( id ) )
                        sleep(1);
                }
            }
            
            fclose(f);
            
            return;
        }
        else
            runConsole();
    }
    
    
    const char* help_message =
    "Samson is a distributed platform to process big-data sources. It has been specially\n"
    "designed to process the output of log-systems like CDRs or DPI's output\n"
    "\n"
    "For more help type help <command>\n"
    "\n"
    "General platform commands:             ls_operations, ls_datas, reload_modules, set, unset \n"
    "                                       ls_local , rm_local , ls_operation_rates\n"
    "\n"
    "Getting information from platform:     info , ps , ls_modules, engine_show , ps_network\n"
    "\n"
    "Batch processing commands:              ls , add , rm , mv , clear , clear_jobs , kill \n"
    "                                        upload , download \n"
    "\n"
    "Getting info for batch processing:      ps_jobs , ps_tasks \n"
    "\n"
    "Stream processing commands:            run_stream_operation, push , pop \n"
    "                                       add_stream_operation , rm_stream_operation , set_stream_operation_property \n"
    "                                       rm_queue , cp_queue \n"
    "                                       set_queue_property\n" 
    "\n"
    "Getting info for stream processing:    ls_queues, ls_queues_info,  ps_stream \n"
    "                                       ls_stream_operation , ls_block_manager \n"
    "\n"
    ;
    
    const char* help_commands[][2] =                                            
    {                                                                           
        { "info"                    , "info select/xml/value [select_query [-limit n]]     Tool used to query the general xml based monitorization.\n" 
        
                                      "For the select option:"
                                      "                                     [select_query] Query in xpath notation"
                                      "                                     [-limit n] Limit the depth of the information tree"
        },
        { "ls"                      , "ls          Show a list of all the key-value sets" } ,                
        { "ls_local"                , "ls_local      Show a list of current directory with relevant information about local data-sets" } ,                
        { "rm_local"                , "rm_local dir_name      Remove a local directory and all its contents" } ,                
        { "ls_modules"              , "ls_modules          Show a list of modules installed at controller, workers and delilah" } ,                
        { "rm"                      , "rm <set>    Remove a key-value set. Usage rm <set>" },              
        { "mv"                      , "mv <set> <new_set>     Change the name of a particular key-value set"},
        { "clear"                   , "clear <set>    Clear the content of a particular key-value set"},
        { "ls_processes"            , "ls_processes     Shows a list with all the processes ( batch and stream ) running on all the workers"},

        { "ls_operations"           , "ls_operations [op_name]      Show a list of operations.\n" 
                                      "                             If [op_name] is specified, only operations starting with op_name are shown"},
        { "ls_datas"                , "ls_datas [data_name]      Show a list of data-types.\n" 
                                      "                          If [data_name] is specified, only data-types starting with data_name are shown"},
        { "ps"                      , "ps [-clear] [id]     Show all current processes running (or finished) on delilah client\n"
            "                     [-clear] removes finished or error marked processes\n"
            "                     [id] get more help about a particular process" } ,                
        
        { "ps_network"              , "ps_network           Get information about network connections in the SAMSON cluster"},
        { "reload_modules"          , "reload_modules        reload_modules used by the platform"},
        
        
        { "add"                     , "add <set> <key-format> <value-format>\n"
            "add <set> -txt\n"
            "Add a key-value set with a particular format. ( -txt create text sets )"
        	"add <set> -f\n"
        		        "Suppress error if <set> already exists (with the same key-value types). If different key-value types, still error"},
        
        { "set"                     , "set <var> <value>       Set environment variables ( all operations can use them )" },
        { "unset"                   , "unset <var>             Remove an environment variable " },
        
        { "ls_operations"           , "ls_operations [-begin X] [-end -X]      Get a list of the available operations ( parser, map, reduce, script, etc)"},
        { "ls_datas"                , "ls_datas [-begin X] [-end -X]           Get a list of available data types for keys and values"},
        
        { "ps_jobs"                 , "ps_jobs           Get a list of running or finished (batch processing) jobs in the platform" },
        { "ps_tasks"                , "ps_tasks          Get a list of running batch processing tasks on controller and workers" },
        { "clear_jobs"              , "clear_jobs (cj)   Clear finished or error marked jobs in the platform" },
        { "kill"                    , "kill (k)  job_id  Kill the job with job_id and all its sub-tasks" },
        
        { "upload"                  , "upload  <local_file_name> <set>       Load txt files to the platform, available for batch processing" },
        { "download"                , "download  <set> <local_directory_name> [-force]     Download the dataset from the platform to local directory\n" 
            "                                               [-force] Remove local directory first"  },
        
        { "push"                    , "push <local_file_or_dir> <queue>        Push content of a local file/directory to a queue"},
        { "pop"                     , "pop <queue> <local_dir>                 Pop content of a queue to a local directory. Also working for binary queues. Use samsonCat to check content"},
        
        { "add_stream_operation"    , "add_stream_operation <name> <operation> <input .. output queues>     Add an operation to automatically process data from input queues to output queues"},
        
        { "rm_stream_operation"     , "rm_stream_operation <name>    Remove a previously introduced operation with add_stream_operation"},

        { "set_stream_operation_property"     , "operation not yet available"},
        
        { "ls_stream_operations"    , "ls_stream_operation           Show a list of active stream operations"},
                
        { "rm_queue"                , "rm_queue <queue>         Remove a queue" },
                
        { "ls_queues"               , "ls_queues        Show a list of current queues in all the workers" },
        { "ls_queues_info"               , "ls_queues_info        Show a list of current queues in all the workers with more information about blocks" },
        
        { "ps_stream"               , "ps_stream        Get a list of current stream tasks"},
        
        { "engine_show"             , "engine_show      Show a status information of the engine resources running on all the elements of SAMSON platform"},
    
        { "run_stream_operation"    , "run_stream_operation [op_name] [queues...] [-clear_inputs]         Run a particular operation over stream queues"},
        { "ls_stream_operation"    , "ls_stream_operation  List all stream operations scheduled with add_stream_operation"},
        
        { "ls_block_manager"        , "ls_block_manager             Get information about the block-manager for each worker" },
    
        { "cp_queue"                , "cp_queue  <from_queue> <to_queue>            Copy contents of one queue to another"},
        { "ls_operation_rates"      , "ls_operation_rates           get a list of statistics about operations in the platform"},
        { "set_queue_property"      , "set_queue_property queue property value               Specify a property of a particular queue"},
        { "ls_stream_activity"      , "ls_stream_activity       show a list of the last activity logs about automatic stream processing"},
        
        { "connect_to_queue"        , "connect_to_queue queue           Connect to a particular queue to receive live data from SAMSON"},
        { "disconnect_from_queue"   , "disconnect_from_queue            Disconnects from a particular queue to not receive live data anymore"},
        
        { NULL , NULL }   
    };
    

    
    // Generic list of information from the xml document
    
    std::string generic_node_to_string_function( const pugi::xml_node& node )
    {
        std::ostringstream output;
        pugi::str( node , 0 ,  output , 1000 );
        return output.str();
    }
      
    

    
    
    size_t DelilahConsole::runAsyncCommand( std::string command )
	{
		
		au::CommandLine commandLine;
		commandLine.set_flag_string("name", "null");
		commandLine.set_flag_string("begin", "null");           
		commandLine.set_flag_boolean("plain");              // Flag to indicate plain ( not used ) 
		commandLine.set_flag_boolean("gz");                 // Flag to indicate compression ( not used )
		commandLine.set_flag_int("threads",4);              // Specify number of threads ( not used )
        commandLine.set_flag_boolean("force");              // Force to remove directory if exist before in pop operations
        commandLine.set_flag_boolean("clear");              // Used in the ps command
        commandLine.set_flag_int("limit", 1000);
		commandLine.parse( command );
        
		std::string mainCommand;
        
		if( commandLine.get_num_arguments() == 0)
		{
            //clear();
			return 0;	// Zero means no pending operation to check
		}
		else
			mainCommand = commandLine.get_argument(0);
        
		if ( commandLine.isArgumentValue(0,"help","h") )
		{
			
            if( commandLine.get_num_arguments() == 1 )
            {
                
                std::ostringstream output;
                output << "\n";
                output << "SAMSON v " << SAMSON_VERSION << "\n";
                output << "==============================================================\n";
                output << "\n";
                output << help_message;
                output << "\n";
                
                writeOnConsole( au::indent( output.str() ) );
                return 0;
            }
            else
            {
                std::string command = commandLine.get_argument(1);
                
                // Search into defined commands for help
                int  i = 0 ;
                while( help_commands[i][0] != NULL )
                {
                    if( strcmp( command.c_str()  , help_commands[i][0] ) == 0 )
                    {
                        std::ostringstream output;
                        output << "\n";
                        output << help_commands[i][1];
                        output << "\n";
                        writeOnConsole( au::indent( output.str() ) );
                        return 0;
                    }
                    i++;
                }
                
                writeWarningOnConsole(au::str("\nNot help for %s\n\n" , command.c_str()));
                return 0;
            }
            
		}
		
		if ( commandLine.isArgumentValue(0, "quit", "") )
		{
			Console::quitConsole();	// Quit the console
			return 0;
		}
		
        
		if ( mainCommand == "set")
		{
			if ( commandLine.get_num_arguments() == 1)
			{
				// Only set, we show all the defined parameters
				std::ostringstream output;
				output << "Environent variables:\n";
				output << "------------------------------------\n";
				output << environment.toString();
				output << "\n";
				output << "\n";
				writeOnConsole( output.str() );
				return 0;
			}
			
			if ( commandLine.get_num_arguments() < 3 )
			{
				writeErrorOnConsole("Usage: set name value.\n");
				return 0;
			}
			
			// Set a particular value
			std::string name = commandLine.get_argument(1);
			std::string value = commandLine.get_argument(2);
			
			environment.set( name , value );
			std::ostringstream o;
			o << "Environment variable " << name << " set to " << value << "\n";
			writeOnConsole( o.str() );
			return 0;
		}
		
		if ( mainCommand == "unset")
		{
			if ( commandLine.get_num_arguments() < 2 )
			{
				writeErrorOnConsole("Usage: unset name.\n");
				return 0;
			}
			
			// Set a particular value
			std::string name = commandLine.get_argument(1);
			environment.unset( name );
			
			std::ostringstream o;
			o << "Environment variable " << name << " removed\n";
			writeOnConsole( o.str() );
			
			return 0;
		}	
        
		if ( mainCommand == "trace")
		{
			if ( commandLine.get_num_arguments() == 1)
			{
				if( trace_on )
					writeOnConsole( "Traces are activated" );
				else
					writeOnConsole( "Traces are NOT activated" );
				return 0;
			}
			
			if( commandLine.get_argument(1) == "on" )
			{
				trace_on = true;
				writeOnConsole( "Traces are now activated" );
				return 0;
			}
			if( commandLine.get_argument(1) == "off" )
			{
				trace_on = false;
				writeOnConsole( "Traces are now NOT activated" );
				return 0;
			}
			
			writeErrorOnConsole("Usage: trace on/off");
			return 0;
		}
		
		
		if ( mainCommand == "clear_components" )
		{
			// Clear completed upload and download process
			clearComponents();
            
            writeOnConsole("Clear components OK");
            
			return 0;
		}
		
		if ( mainCommand == "download")
		{
			if ( commandLine.get_num_arguments() < 3)
			{
				writeErrorOnConsole( "Error: Usage: download data_set_name local_file_name\n");
				return 0;
			}
			
			std::string queue_name = commandLine.get_argument(1);
			std::string fileName = commandLine.get_argument(2);
            
			size_t id = addDownloadProcess(queue_name, fileName , commandLine.get_flag_bool("force"));
            
			std::ostringstream o;
			o << "[ " << id << " ] Download data process started.";
			writeWarningOnConsole(o.str());
			return id;
		}
		
		if ( mainCommand == "ps" )
		{
            
            if( commandLine.get_flag_bool("clear") )
                clearComponents();
            
            if( commandLine.get_num_arguments() > 1 )
            {
                size_t id = atoi( commandLine.get_argument(1).c_str() );
                
                DelilahComponent *component = components.findInMap( id );
                if( !component )
                    writeErrorOnConsole( au::str("Unkown process with id %d", id ) );
                else
                {
                    std::ostringstream output;
                    output << "------------------------------------------------\n";
                    output << " Process " << id << " " << component->cronometer.str() << "\n";
                    output << "------------------------------------------------\n";
                    output << "\n";
                    if( component->error.isActivated() )
                        output << "ERROR: " << component->error.getMessage() << "\n";
                    output << "\n";
                    output << component->getStatus();
                    output << "\n";
                    writeOnConsole(output.str());
                    
                }
                
                return 0;
            }
            
			std::ostringstream output;
			output << getListOfComponents();
			writeOnConsole(output.str());
			
			return 0;
			
		}
          
        // Upload and download operations
        // ------------------------------------------------------------------------------------
        
        
		if( mainCommand == "upload" )
		{
			if( commandLine.get_num_arguments() < 3)
			{
				writeErrorOnConsole("Usage: upload file <file2> .... <fileN> queue");
				return 0;
			}
			
			std::vector<std::string> fileNames;
			for (int i = 1 ; i < (commandLine.get_num_arguments()-1) ; i++)
			{
				std::string fileName = commandLine.get_argument(i);
				
				struct stat buf;
				stat( fileName.c_str() , &buf );
				
				if( S_ISREG(buf.st_mode) )
				{
					if( trace_on )
					{
                        
						std::ostringstream message;
						message << "Including regular file " << fileName;
						writeOnConsole( message.str() );
					}
					
					fileNames.push_back( fileName );
				}
				else if ( S_ISDIR(buf.st_mode) )
				{
					if( trace_on )
					{
						std::ostringstream message;
						message << "Including directory " << fileName;
						writeOnConsole( message.str() );
					}
					
					{
						// first off, we need to create a pointer to a directory
						DIR *pdir = opendir (fileName.c_str()); // "." will refer to the current directory
						struct dirent *pent = NULL;
						if (pdir != NULL) // if pdir wasn't initialised correctly
						{
							while ((pent = readdir (pdir))) // while there is still something in the directory to list
								if (pent != NULL)
								{
									std::ostringstream localFileName;
									localFileName << fileName << "/" << pent->d_name;
                                    
									struct stat buf2;
									stat( localFileName.str().c_str() , &buf2 );
									
									if( S_ISREG(buf2.st_mode) )
										fileNames.push_back( localFileName.str() );
									
								}
							// finally, let's close the directory
							closedir (pdir);						
						}
					}
				} 
				else
				{
					if( trace_on )
					{
						std::ostringstream message;
						message << "Skipping " << fileName;
						writeOnConsole( message.str() );
					}
				}
			}
			
			std::string queue = commandLine.get_argument( commandLine.get_num_arguments()-1 );
            
			bool compresion = false;
			/*
			 // Compression deactivated temporary
             if( commandLine.get_flag_bool("gz") )
             compresion = true;
             if( commandLine.get_flag_bool("plain") )
             compresion = false;
			 */
			
			int max_num_thread = commandLine.get_flag_int("threads"); 
			
			size_t id = addUploadData(fileNames, queue,compresion , max_num_thread);
			
			//std::ostringstream o;
			//o << "[ " << id << " ] Load data process started with " << fileNames.size() << " files";
			//writeWarningOnConsole(o.str());
            
			return id;
		}
		
        
        // Push data to a queue
        
        if( mainCommand == "push" )
		{
			if( commandLine.get_num_arguments() < 3 )
			{
				writeErrorOnConsole("Usage: push file <file2> .... <fileN> queue1,queue2,queue3 ");
				return 0;
			}
			
			std::vector<std::string> fileNames;
			for (int i = 1 ; i < (commandLine.get_num_arguments()-1) ; i++)
			{
				std::string fileName = commandLine.get_argument(i);
				
				struct stat buf;
				stat( fileName.c_str() , &buf );
				
				if( S_ISREG(buf.st_mode) )
				{
					if( trace_on )
					{
                        
						std::ostringstream message;
						message << "Including regular file " << fileName;
						writeOnConsole( message.str() );
					}
					
					fileNames.push_back( fileName );
				}
				else if ( S_ISDIR(buf.st_mode) )
				{
					if( trace_on )
					{
						std::ostringstream message;
						message << "Including directory " << fileName;
						writeOnConsole( message.str() );
					}
					
					{
						// first off, we need to create a pointer to a directory
						DIR *pdir = opendir (fileName.c_str()); // "." will refer to the current directory
						struct dirent *pent = NULL;
						if (pdir != NULL) // if pdir wasn't initialised correctly
						{
							while ((pent = readdir (pdir))) // while there is still something in the directory to list
								if (pent != NULL)
								{
									std::ostringstream localFileName;
									localFileName << fileName << "/" << pent->d_name;
                                    
									struct stat buf2;
									stat( localFileName.str().c_str() , &buf2 );
									
									if( S_ISREG(buf2.st_mode) )
										fileNames.push_back( localFileName.str() );
									
								}
							// finally, let's close the directory
							closedir (pdir);						
						}
					}
				} 
				else
				{
					if( trace_on )
					{
						std::ostringstream message;
						message << "Skipping " << fileName;
						writeOnConsole( message.str() );
					}
				}
			}
			
			std::string queues_txt = commandLine.get_argument( commandLine.get_num_arguments()-1 );
            
            std::vector<std::string> queues = au::split( queues_txt , ',' );
            
			size_t id = addPushData(fileNames, queues );
			
			return id;
		}
        
        
        // Push data to a queue
        
        if( mainCommand == "pop" )
		{
			if( commandLine.get_num_arguments() < 3 )
			{
				writeErrorOnConsole("Usage: pop queue fileName");
				return 0;
			}
            
            std::string queue_name  = commandLine.get_argument(1);
            std::string fileName    = commandLine.get_argument(2);
            
            bool force_flag = commandLine.get_flag_bool("force");
            
			size_t id = addPopData( queue_name ,  fileName , force_flag );
			
			return id;
		}
        
        
        // WorkerCommands
        std::string main_command = commandLine.get_argument(0);
        

        if( main_command == "run_stream_operation" )
        {
            return sendWorkerCommand( command , NULL );
        }

        if( main_command == "add_stream_operation" )
        {
            return sendWorkerCommand( command , NULL );
        }

        if( main_command == "rm_stream_operation" )
        {
            return sendWorkerCommand( command , NULL );
        }
        
        if( main_command == "set_stream_operation_property" )
        {
            return sendWorkerCommand( command , NULL );
        }
        
        if( main_command == "remove_all_stream" )
        {
            return sendWorkerCommand( command , NULL );
        }

        if( main_command == "connect_to_queue" )
        {
            return sendWorkerCommand( command , NULL );
        }
        
        if( main_command == "disconnect_form_queue" )
        {
            return sendWorkerCommand( command , NULL );
        }
        
        if( main_command == "wait" )
        {
            return sendWorkerCommand( command , NULL );
        }
        
        if( main_command == "run_stream_update_state" )
        {
            return sendWorkerCommand( command , NULL );
        }
        
        // Command to remove queues 
        if( main_command == "rm_queue" )
        {
            if( commandLine.get_num_arguments() < 2 )
            {
                writeErrorOnConsole( au::str("Usage: rm_queue queue" ) );
                return 0;
            }
            else
                return sendWorkerCommand( command , NULL );
        }

        // Command to remove queues 
        if( main_command == "set_queue_property" )
        {
            if( commandLine.get_num_arguments() < 4 )
            {
                writeErrorOnConsole( au::str("Usage: set_queue_propert queue property value" ) );
                return 0;
            }
            else
                return sendWorkerCommand( command , NULL );
        }
        
        // Command to remove queues 
        if( main_command == "cp_queue" )
        {
            if( commandLine.get_num_arguments() < 3 )
            {
                writeErrorOnConsole( au::str("Usage: cp_queue form_queue to_queue" ) );
                return 0;
            }
            else
                return sendWorkerCommand( command , NULL );
        }
        
        
        
        // Command to play / pause statess
        if( main_command == "pause_queue" )
        {
            if( commandLine.get_num_arguments() < 2 )
            {
                writeErrorOnConsole( au::str("Usage: pause_queue queue" ) );
                return 0;
            }
            else
                return sendWorkerCommand( command , NULL );
            
        }
        
        if( main_command == "play_queue" )
        {
            if( commandLine.get_num_arguments() < 2 )
            {
                writeErrorOnConsole( au::str("Usage: play_queue queue" ) );
                return 0;
            }
            else
                return sendWorkerCommand( command , NULL );
        }
        
        if ( ( mainCommand == "info" ) || ( mainCommand == "i" ) )
        {
            
            if( commandLine.get_num_arguments() < 2 )
            {
                writeErrorOnConsole("Usage info_query select-query");
                return 0;
            }
            
            std::string query = commandLine.get_argument(1);
            writeWarningOnConsole(au::str("Running select %s" , query.c_str()));
            std::string result = getQuery( query , commandLine.get_flag_int("limit") );
            writeOnConsole(result );
            
            return 0;
            
            
        }
        
        if( (main_command == "s") || (main_command == "status") )
        {
            // Show status
            int controller_update_time = getUpdateSeconds();
            
            if( controller_update_time > 20 )
            {
                std::ostringstream output;
                output << "Information not updated correctly ( " << au::time_string( controller_update_time ) << " )\n"; 
                writeErrorOnConsole( output.str() );
                return 0;
            }
            
            
            std::ostringstream output;
            output << "SAMSON CLuster:\n";
            output << "------------------------------------------------------------\n";
            output << "Information updated correctly ( " << au::time_string( controller_update_time ) << " )\n"; 
            output << "------------------------------------------------------------\n";

            std::string txt = getStringInfo( "/update_time" , getUpdateTimeInfo, i_controller | i_no_title ); 
            output << txt;
            
            output << "------------------------------------------------------------\n";
            
            std::string txt2 = getStringInfo("/engine_system", getEngineSimplifiedSystemInfo, i_worker ); 
            output << txt2;
            
            writeOnConsole( output.str() );
            
            return 0;
        }
        

        if( main_command == "info_command" )
        {
            std::string txt = infoCommand( command );
            writeOnConsole( txt );
            return 0;
        }
        

        if( main_command == "rm_local" )
        {
            if( commandLine.get_num_arguments() < 2 )
            {
                writeErrorOnConsole( "Usage: rm_local <dir>");
                return 0;
            }

            au::ErrorManager error;
            au::removeDirectory( commandLine.get_argument(1) , error );
            
            if( error.isActivated() )
                writeErrorOnConsole( error.getMessage() );
            else
                writeWarningOnConsole("OK");
            return 0;
        }

        
        if( (main_command=="ls") || ( main_command.substr(0,3) == "ls_" ) || ( main_command.substr(0,3) == "ps_" ) || ( main_command == "engine_show" ) )
        {
            writeOnConsole( info( command ) );
            return 0;
        }
        
        
        if( main_command == "save_xml" )
        {
            if( commandLine.get_num_arguments() < 2 )
            {
                writeErrorOnConsole("Usage: save_xml file");
                return 0;
            }
            
            std::string fileName = commandLine.get_argument(1);
            
            FILE *file = fopen( fileName.c_str() , "w" );
            if( !file )
            {
                writeErrorOnConsole("Not possible to open file " + fileName );
                return 0;
            }
            
            std::string txt = xmlString();
            
            fwrite( txt.c_str() , txt.length() , 1 , file );
            
            fclose( file );
            return 0;
            
            
        }
        
		// By default, we consider a normal command sent to controller
		return sendCommand( command , NULL );
        
	}
	
	int DelilahConsole::_receive(int fromId, Message::MessageCode msgCode, Packet* packet)
	{
		std::ostringstream  txt;
        
		switch (msgCode) {
				
            case Message::Trace:
            {
                std::string _text   = packet->message->trace().text();
                std::string file    = packet->message->trace().file();
                std::string fname   = packet->message->trace().fname();
                std::string stre    = packet->message->trace().stre();
                
                //writeOnConsole( au::str("Trace: %s", _text.c_str() ) );
                if( trace_on )
                {
                    
                    lmFdRegister(1, DEF1, "DEF", "stdout", NULL);
                    
                    
                    //if (lmOk(packet->message->trace().type(), packet->message->trace().tlev() ) == LmsOk)
                    {
                        
                        lmOut(
                              (char*)_text.c_str(),   
                              (char) packet->message->trace().type() , 
                              file.c_str(), 
                              packet->message->trace().lineno() , 
                              fname.c_str(), 
                              packet->message->trace().tlev() , 
                              stre.c_str() 
                              );
                    }
                    
                    //std::cerr << "TRACE: " << file << " ( " << fname << " ): " << _text << "\n";  
                    
					lmFdUnregister(1);
                    
                    //return 0;    
                }
                
            }
                break;
                
			case Message::CommandResponse:
			{
                // No more messages of this type. All managed by DelilahCommandComponent
/*				
				if( packet->message->command_response().has_new_job_id() )
				{
					std::ostringstream message;
					message << "Job scheduled [" << packet->message->command_response().new_job_id() << "] ";
					message << " ( " << packet->message->command_response().command().command() << ")";
					writeWarningOnConsole( message.str() );
					return 0;
				}
				
				if( packet->message->command_response().has_finish_job_id() )
				{
					std::ostringstream message;
					message << "Job finished  [" << packet->message->command_response().finish_job_id() << "] ";
					message << " ( " << packet->message->command_response().command().command() << ")";
					message << " ["<< au::time_string( packet->message->command_response().ellapsed_seconds() ) << "] ";
					writeWarningOnConsole( message.str() );
					return 0;
				}
                
				if( packet->message->command_response().has_error_job_id() )
				{
					std::ostringstream message;
					message << "Job finished with error [" << packet->message->command_response().error_job_id() << "] ";
					message << " ( " << packet->message->command_response().command().command() << ")\n\n";
					
					if( packet->message->command_response().has_error_message() )
						message <<  packet->message->command_response().error_message();
					writeErrorOnConsole( message.str() );
					return 0;
				}
				
				if( packet->message->command_response().has_error_message() )
					writeErrorOnConsole( packet->message->command_response().error_message()  );
*/                
			}
				break;
				
                
    
			default:
				txt << "Unknwn packet received\n";
                
                LM_X(1, ("Unknown packet received at delilahConsole"));
				break;
		}
		
		
		return 0;
	}	

    void DelilahConsole::delilahComponentStartNotification( DelilahComponent *component)
    {
        std::ostringstream o;
        o << "Local process started: " << component->getIdAndConcept() << "\n";
        
        if( component->error.isActivated() )
            writeErrorOnConsole(o.str());        
        else
            writeWarningOnConsole(o.str());        
    }
    
    void DelilahConsole::delilahComponentFinishNotification( DelilahComponent *component )
    {
        std::ostringstream o;
        o << "Local process finished: " << component->getIdAndConcept() << "\n";

        // Include error if any
        if( component->error.isActivated() )
            o << "\n" << component->error.getMessage() << "\n";
        
        if( component->error.isActivated() )
            writeErrorOnConsole(o.str());        
        else
            writeWarningOnConsole(o.str());        
    }
    


    
}
