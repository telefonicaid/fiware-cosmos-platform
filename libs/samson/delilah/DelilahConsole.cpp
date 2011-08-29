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
#include "au/Format.h"						// au::Format

#include "pugi/pugi.h"                  // pugi::Pugi
#include "pugi/pugixml.hpp"             // pugi:...

#include "samson/network/Packet.h"						// ss:Packet

#include "samson/common/Info.h"                     // samson::Info

#include "samson/module/ModulesManager.h"           // samson::ModulesManager

#include "samson/delilah/Delilah.h"					// samson::Delailh
#include "samson/delilah/DelilahConsole.h"				// Own interface

#include "DelilahUploadDataProcess.h"		// samson::DelilahUpLoadDataProcess
#include "DelilahDownloadDataProcess.h"		// samson::DelilahDownLoadDataProcess
#include "engine/MemoryManager.h"				// samson::MemoryManager
#include "samson/common/EnvironmentOperations.h"						// Environment operations (CopyFrom)
#include "samson/module/samsonVersion.h"		// SAMSON_VERSION


#include "samson/stream/BlockManager.h"     // samson::stream::BlockManager

#include "PushComponent.h"  

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
                op.addQueueOptions( NULL );
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
        "General platform commands:             operations, datas, reload_modules, set, unset\n"
        "\n"
        "Getting information from platform:     ps , ls_processes\n"
        "\n"
        "Bath processing commands:              ls , add , rm , mv , clear , jobs , clear_jobs , kill , upload , download \n"
        "\n"
        "Getting info for bath processing:      ??? \n"
        "\n"
        "Stream processing commands:            push , pop , add_stream_operation , rm_stream_operation , set_stream_operation_property\n"
        "                                       push_state_to_queue , rm_queue , rm_state , play_state , pause_state\n" 
        "\n"
        "Getting info for stream processing:    ls_queues , ls_states , ps_stream , ls_stream_operation \n"
        "\n"
    ;

    const char* help_commands[][2] =                                            
    {                                                                           
        { "ls"                      , "ls          Show a list of all the key-value sets" } ,                
        { "rm"                      , "rm <set>    Remove a key-value set. Usage rm <set>" },              
        { "mv"                      , "mv <set> <new_set>     Change the name of a particular key-value set"},
        { "clear"                   , "clear <set>    Clear the content of a particular key-value set"},
        { "ls_processes"            , "ls_processes     Shows a list with all the processes ( batch and stream ) running on all the workers"},
        
        { "ps"                      , "ps [-clear] [id]     Show all current process running on delilah client\n" 
                                      "                     [-clear] removes finished or error processes\n"
                                      "                     [id] get more help about a particular process" } ,                
        
        { "reload_modules"          , "reload_modules        reload_modules used by the platform"},
                
        
        { "add"                     , "add <set> <key-format> <value-format>\n"
                                      "add <set> -txt\n"
                                      "Add a key-value set with a particular format. ( -txt create text sets )" },              

        { "set"                     , "set <var> <value>       Set environment variables ( all operations can use them )" },
        { "unset"                   , "unset <var>             Remove a environment variables " },

        { "operations"              , "operations [-begin X] [-end -X]      Get a list of the available operations ( parser, map, reduce, script, etc)"},
        { "datas"                   , "datas [-begin X] [-end -X]           Get a list of available data types for keys and values"},
                        
        { "jobs"                    , "jobs (j)          Get a list of running jobs" },
        { "clear_jobs"              , "clear_jobs (cj)   Clear finish or error jobs" },
        { "kill"                    , "kill (k)          Kill a particular job and all its sub-tasks" },

        { "upload"                  , "upload  <local_file_name> <set>       Load txt files to the platform" },
        { "download"                , "download  <set> <local_file_name> [-force]     Download txt sets from the platform to local file\n" 
                                      "                                               [-force] Remove local directory first"  },
        
        { "push"                    , "push <local_file_or_dir> <queue>        Push content of a local file/directory to a queue"},
        { "pop"                     , "pop <queue> <local_dir>                 Pop content of a queue to a local dir. Use samsonCat to check content in binary data"},
        
        { "add_stream_operation"    , "add_stream_operation <name> <operation> <input .. output queues>\n"
                                      "\n"
                                      "Add an operation to automatically process data from queues to queues"},
        
        { "rm_stream_operation"     , "rm_stream_operation <name>    Remove a previously introducted operation with add_stream_operation"},
        
        { "ls_stream_operations"    , "ls_stream_operation           Show a list of active stream operations"},
        
        { "push_state_to_queue"     , "push_state_to_queue <state> <queue>     Push content of a state ( used in stream reduce ) to a queue"},
        
        { "rm_queue"                , "rm_queue <queue>         Remove a queue" },
        { "rm_state"                , "rm_state <state>         Remove a state ( stream reduce operations ) " },
        
        { "pause_state"             , "pause_state <state>        Pause an state, so no new stream reduce operation are scheduled " },
        { "play_state"              , "play_state <state>         Cancel pause_state command over a state " },
        
        { "ls_queues"               , "ls_queues        Show a list of current queues in all the workers" },
        { "ls_states"               , "ls_states        Show a list of current states in all the workers ( used in the stream reduce operations )" },
        
        { "ps_stream"               , "ps_stream        Get a list of current stream tasks"},
        
        { NULL , NULL }   
    };

    
    std::string getBLockListString( pugi::node node )
    {
        
        
        size_t size_total =  pugi::getUInt64( node , "size_total" );
        size_t size_on_memory =  pugi::getUInt64( node , "size_on_memory" );
        size_t size_on_disk =  pugi::getUInt64( node , "size_on_disk" );

        int num_blocks = pugi::getInt( node , "num_blocks" );
        size_t kvs = pugi::getUInt64( node , "kvs" );
        size_t size = pugi::getUInt64( node , "size" );
        
        if( ( kvs == 0 ) && (size == 0) )
            return "Empty";
        
        std::ostringstream output;
        output << num_blocks << " blocks with ";
        output << au::str( kvs , "kvs" ) << " in ";
        output << au::str( size , "bytes" )  ;
        output << " ";
        if( size_total > 0 )
        {
            double p_memory = (double) size_on_memory / (double) size_total;
            double p_disk = (double) size_on_disk / (double) size_total;
            
            output << au::Format::percentage_string( p_memory ) << " on memory & " << au::Format::percentage_string( p_disk ) << " on disk";
        }
        
        return output.str();
    }
    
    
    /*
     output << "---------------------------------------------------------------------\n";
     output << "\n";
     output << " trace       Activate or deactivate traces\n";
     output << "             Usage: trace on/off\n";
     output << "---------------------------------------------------------------------\n";
     output << "\n";
     output << " info                General information about Controller and Workers status\n";
     output << "                     Usage: info\n";
     output << "\n";
     output << " info_cores          Similar to info, with graphical representation for Worker status\n";
     output << "                     Usage: info_cores\n";
     output << "\n";
     output << " info_modules        Show current modules used in all the nodes \n";
     output << "\n";
     output << " info_disk_manager   Information about disk operations (read and write) for every Worker\n";
     output << "\n";
     output << " info_engine         Information about the task scheduling in Engines\n";
     output << "\n";
     output << " info_full           A very detailed information for every Worker\n";
     output << "\n";
     output << " info_load_data_manager      Similar to info, with information of uploads and downloads per Worker\n";
     output << "\n";
     output << " info_memory_manager Similar to info, with detailed information of the memory assigned to every Worker\n";
     output << "\n";
     output << " info_net            Detailed information about the network traffic between nodes in the cluster\n";
     output << "\n";
     output << " info_process_manager        For every Worker, it shows the list of operations in every possible state (Running, Halting, Queued)\n";
     output << "\n";
     output << " info_task_manager   For the Controller, it gives information about the jobs completed and running, and the tasks in the case of scripts\n;";
     output << "                     For every Worker, for every task, it shows the component process and their state\n";
     output << "\n";
     output << "---------------------------------------------------------------------\n";
     
     */

        size_t DelilahConsole::runAsyncCommand( std::string command )
	{
		
		au::CommandLine commandLine;
		commandLine.set_flag_string("name", "null");
		commandLine.set_flag_string("begin", "null");
		commandLine.set_flag_boolean("plain");
		commandLine.set_flag_boolean("gz");			// Flag to indicate compression
		commandLine.set_flag_int("threads",4);
        commandLine.set_flag_boolean("force");      // Force to remove directory if exist before in pop operations
        commandLine.set_flag_boolean("clear");      // Used in the ps command
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
		
		if ( mainCommand == "load" )
		{
			std::ostringstream output;
			output << getListOfLoads();
			writeOnConsole(output.str());
			
			return 0;
			
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
                    output << " Process " << id << "\n";
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
        
		if (mainCommand == "netstate")
		{
			std::string s;

			s = network->getState(command);
			writeOnConsole(s);

			return 0;
		}
        
        // Show info based of the periodically received information about status
        // ------------------------------------------------------------------------------------
        
        if( mainCommand == "w" )
        {
            showInfo("info_full");
            return 0;
        }
        
        if( mainCommand == "info" )
        {
            au::TokenTaker tt( &info_lock );
            
            if( commandLine.get_num_arguments() == 1 )
            {
                writeErrorOnConsole("Usage: info txt/xml/select/values/num/str/double [options]");
                return 0;
            }
            
            // Main xml command
            std::string command = commandLine.get_argument(1);
            
            if( command == "xml" )
            {
                writeOnConsole( pugi::str( doc ) );
                return 0;
            }
            
            if ( command == "txt" )
            {
                writeOnConsole( xml_info );
                return 0;
            }
            
            if( command == "values" )
            {
                if( commandLine.get_num_arguments() < 3 )
                {
                    writeErrorOnConsole("Usage info_query select select-query");
                    return 0;
                }
                
                std::string query = commandLine.get_argument(2);
                writeWarningOnConsole(au::str("Running select %s" , query.c_str()));
                
                pugi::ValuesCollection vc = pugi::values(doc, query);
                
                writeWarningOnConsole( "Values: " + vc.str() );

                return 0;
                
            }
            
            if ( command == "num" )
            {
                if( commandLine.get_num_arguments() < 3 )
                {
                    writeErrorOnConsole("Usage info_query num query");
                    return 0;
                }
                
                std::string query_str = commandLine.get_argument(2);

                size_t result = pugi::UInt64( doc , query_str );

                writeWarningOnConsole(au::str("Running query %s wiht result %lu" , query_str.c_str() , result ) );
                
                return 0;
            }            
            
            if ( command == "double" )
            {
                if( commandLine.get_num_arguments() < 3 )
                {
                    writeErrorOnConsole("Usage info double query");
                    return 0;
                }
                
                std::string query_str = commandLine.get_argument(2);
                
                double result = pugi::Double( doc , query_str );
                
                writeWarningOnConsole(au::str("Running query %s wiht result %f" , query_str.c_str() , result ) );
                
                return 0;
            }            

            if ( command == "str" )
            {
                if( commandLine.get_num_arguments() < 3 )
                {
                    writeErrorOnConsole("Usage info_query num query");
                    return 0;
                }
                
                std::string query_str = commandLine.get_argument(2);
                
                std::string result = pugi::String( doc , query_str );
                
                writeWarningOnConsole(au::str("Running query %s wiht result %s" , query_str.c_str() , result.c_str() ) );
                
                return 0;
            }            
            
            if ( command == "select")
            {
                if( commandLine.get_num_arguments() < 3 )
                {
                    writeErrorOnConsole("Usage info_query select select-query");
                    return 0;
                }
                
                std::string query = commandLine.get_argument(2);
                writeWarningOnConsole(au::str("Running select %s" , query.c_str()));
                
                pugi::xpath_node_set result;
                try {
                    result = doc.select_nodes( query.c_str() );
                } catch (pugi::xpath_exception& ex) 
                {
                    writeErrorOnConsole( au::str( "Error in xpath query: %s" , ex.what() ) );
                    return 0;
                }
                
                // Transform the results into a string
                std::ostringstream result_txt;
                pugi::str( result , result_txt ); // Orignal command is passed for addicional flag promcessing (-full -filter
                
                writeOnConsole(result_txt.str());
                
                return 0;
                
            }
            
            writeWarningOnConsole("Non implemented option");
            
            return 0;
        }
        
        
        // Rest of info stuff
        if( strncmp(mainCommand.c_str(), "info", 4) == 0)
        {
            showInfo( mainCommand );
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
			
			std::ostringstream o;
			o << "[ " << id << " ] Load data process started with " << fileNames.size() << " files";
			writeWarningOnConsole(o.str());
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
			
			std::ostringstream o;
			o << "[ " << id << " ] Push data process started with " << fileNames.size() << " files";
			writeWarningOnConsole(o.str());
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
            
			size_t id = addPopData( queue_name  , "" ,  fileName , force_flag );
			
			std::ostringstream o;
			o << "[ " << id << " ] Pop from queue " << queue_name << " to localFile: " << fileName;
			writeWarningOnConsole(o.str());
			return id;
		}

        if( mainCommand == "pop_state" )
		{
			if( commandLine.get_num_arguments() < 3 )
			{
				writeErrorOnConsole("Usage: pop_state state fileName");
				return 0;
			}
            
            std::string state_name = commandLine.get_argument(1);
            std::string fileName = commandLine.get_argument(2);
            bool force_flag = commandLine.get_flag_bool("force");
            
			size_t id = addPopData( "" , state_name  , fileName , force_flag );
			
			std::ostringstream o;
			o << "[ " << id << " ] Pop from state " << state_name << " to localFile: " << fileName;
			writeWarningOnConsole(o.str());
			return id;
		}
        
        // WorkerCommands
        std::string main_command = commandLine.get_argument(0);
        
        if( main_command == "push_state_to_queue" )
        {
            if( commandLine.get_num_arguments() < 3 )
            {
                writeErrorOnConsole( au::str("Usage: push_state_to_queue state queue" ) );
                return 0;
            }
            else
                return sendWorkerCommand( command , NULL );
        }
        
        
        // Command to remove queues / states 
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
        
        if( main_command == "rm_state" )
        {
            if( commandLine.get_num_arguments() < 2 )
            {
                writeErrorOnConsole( au::str("Usage: rm_state state" ) );
                return 0;
            }
            else
                return sendWorkerCommand( command , NULL );
        }
        
        // Command to play / pause statess
        if( main_command == "pause_state" )
        {
            if( commandLine.get_num_arguments() < 2 )
            {
                writeErrorOnConsole( au::str("Usage: pause_state state" ) );
                return 0;
            }
            else
                return sendWorkerCommand( command , NULL );
            
        }
        
        if( main_command == "play_state" )
        {
            if( commandLine.get_num_arguments() < 2 )
            {
                writeErrorOnConsole( au::str("Usage: play_state state" ) );
                return 0;
            }
            else
                return sendWorkerCommand( command , NULL );
        }
        
        if( main_command == "ls_queues" )
        {
            if( !checkXMLInfoUpdate() )
                return 0;

            std::ostringstream output;
            
            // Get all the workers node
            pugi::ValuesCollection workers_ids = pugi::values(doc, "//worker/id");
            
            for ( size_t w = 0 ; w < workers_ids.size() ; w++ )
            {
                
                output << "------------------------------------------------------------\n";
                output << "Queues at worker " << workers_ids[w] << ":\n";
                output << "------------------------------------------------------------\n";
                
                pugi::xpath_node_set queues  = pugi::select_nodes( doc , "//worker[id=" + workers_ids[w] + "]/stream_manager/queues/queue" );
                
                for ( size_t q = 0 ; q < queues.size() ; q++ )
                {
                    const pugi::xml_node& queue = queues[q].node(); 
                    
                    // Get information for this state    
                    std::string name = pugi::get( queue , "name" );
                    
                    const pugi::node block_list = queue.first_element_by_path("block_list");
                    
                    output << std::setw(15) << name << ": [ "  << getBLockListString (block_list ) << " ]";
                    output << "\n";
                    
                    //pugi::str( state , 2 , output );
                }
                
            }
            
            writeOnConsole( output.str() );
            return 0;
        }
        
        if( main_command == "ls_stream_operation" )
        {
            if( !checkXMLInfoUpdate() )
                return 0;

            std::ostringstream output;
            
            // Get all the workers node
            pugi::xpath_node_set stream_operations  = pugi::select_nodes( doc , "//controller//stream_operation" );
            
            output << "------------------------------------------------------------------------------------------------" << std::endl;
            output << "Stream Operations" << std::endl;
            output << "------------------------------------------------------------------------------------------------" << std::endl;
            
            for ( size_t q = 0 ; q < stream_operations.size() ; q++ )
            {
                // Get the queue
                const pugi::xml_node& stream_operation = stream_operations[q].node(); 
                
                // Get information for this state    
                std::string name = pugi::get( stream_operation , "name" );
                std::string description = pugi::get( stream_operation , "description" );
                
                output << std::setw(15) << name << "  " << description << "\n";
            }
            
            output << "------------------------------------------------------------------------------------------------" << std::endl;
            
            
            
            writeOnConsole( output.str() );
            return 0;
        }
        
        if( main_command == "ls" )
        {
            if( !checkXMLInfoUpdate() )
                return 0;
            
            std::ostringstream output;
            
            int time = getUpdateSeconds();
            output << "Time: " << time << "\n";
            
            // Get all the workers node
            pugi::xpath_node_set queues  = pugi::select_nodes( doc , "//controller//queue" );
            
            output << "------------------------------------------------------------------------------------------------" << std::endl;
            output << "Queues" << std::endl;
            output << "------------------------------------------------------------------------------------------------" << std::endl;
            
            for ( size_t q = 0 ; q < queues.size() ; q++ )
            {
                // Get the queue
                const pugi::xml_node& queue = queues[q].node(); 
                
                // Get information for this state    
                std::string name = pugi::get( queue , "name" );
                
                size_t kvs = pugi::getUInt64( queue , "kvs" );
                size_t size = pugi::getUInt64( queue , "size" );
                
                size_t num_files = pugi::getUInt64( queue , "num_files" );

                std::string key_format = pugi::get( queue , "key_format" );
                std::string value_format = pugi::get( queue , "value_format" );
                
                
                output << std::setw(30) << name;
                output << " ";
                output << au::str( kvs );
                output << " kvs in ";
                output << au::str( size ) << " bytes";
                output << " #File: " << num_files;
                output << " (" << key_format << " " << value_format << ") ";
                output << std::endl;
            }
            output << "------------------------------------------------------------------------------------------------" << std::endl;
            
            
            
            writeOnConsole( output.str() );
            return 0;
        }
        
        if( main_command == "ls_states" )
        {
            
            std::ostringstream output;

            // Get all the workers node
            pugi::ValuesCollection workers_ids = pugi::values(doc, "//worker/id");
            
            for ( size_t w = 0 ; w < workers_ids.size() ; w++ )
            {
                
                output << "------------------------------------------------------------\n";
                output << "Worker " << workers_ids[w] << ":\n";
                output << "------------------------------------------------------------\n";

                pugi::xpath_node_set states  = pugi::select_nodes( doc , "//worker[id=" + workers_ids[w] + "]/stream_manager/states/state" );
                
                for ( size_t s = 0 ; s < states.size() ; s++ )
                {
                    const pugi::xml_node& state = states[s].node(); 

                    // Get information for this state    
                    std::string name = pugi::get( state, "name" );
                    
                    int num_state_items =  pugi::getInt( state , "num_state_items" );
                    
                    const pugi::node state_info = state.first_element_by_path("state_info").first_element_by_path("block_list");
                    const pugi::node input_info = state.first_element_by_path("input_info").first_element_by_path("block_list");
                    
                    output << "State " << name << ": ( " << num_state_items << " state items ) ";
                    output << " State: [ " << getBLockListString( state_info ) << " ] ";
                    
                    
                    output << " Input: [ " << getBLockListString( input_info ) << " ] ";
                    
                    output << "\n";
                    
                    //pugi::str( state , 2 , output );
                }
                
            }
            
            writeOnConsole( output.str() );
            return 0;
        }

        if( main_command == "ps_stream" )
        {
            
            std::ostringstream output;
            
            // Get all the workers node
            pugi::ValuesCollection workers_ids = pugi::values(doc, "//worker/id");
            
            for ( size_t w = 0 ; w < workers_ids.size() ; w++ )
            {
                
                output << "------------------------------------------------------------\n";
                output << "Worker " << workers_ids[w] << ":\n";
                output << "------------------------------------------------------------\n";
                
                pugi::xpath_node_set queue_tasks  = pugi::select_nodes( doc , "//worker[id=" + workers_ids[w] + "]/stream_manager//queue_task" );
                
                for ( size_t s = 0 ; s < queue_tasks.size() ; s++ )
                {
                    const pugi::xml_node& queue_task = queue_tasks[s].node(); 
                    
                    // Get information for this state    
                    std::string id              = pugi::get( queue_task , "id" );
                    std::string description     = pugi::get( queue_task , "description" );
                    
                    const pugi::node block_list = queue_task.first_element_by_path("block_list");
                    
                    output << "Task " << id << " : " << description << "\n";
                    output << "            -> Input " << getBLockListString( block_list ) << "\n";
                    
                    //pugi::str( state , 2 , output );
                }
                
            }
            
            writeOnConsole( output.str() );
            return 0;            
        }
        
        if( main_command == "ls_processes" )
        {
            showInfo("info_process_manager");
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
					message << " ["<< au::Format::time_string( packet->message->command_response().ellapsed_seconds() ) << "] ";
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

				if( packet->message->command_response().has_queue_list() )
				{
					
					// Check if it is a -all command
					au::CommandLine cmdLine;
					cmdLine.set_flag_boolean("all");
					cmdLine.parse(packet->message->command_response().command().command() );
					
					if( cmdLine.get_flag_bool("all") )
					{
						// Copy the list of queues for auto-completion
                        au::TokenTaker tt( &info_lock );
						
						if( ql )
							delete ql;
						ql = new network::QueueList();
						ql->CopyFrom( packet->message->command_response().queue_list() );
						
						
					}
					else
					{
						showQueues( packet->message->command_response().queue_list() );
					}
					
				}
								
				if( packet->message->command_response().has_data_list() )
					showDatas( packet->message->command_response().data_list() );
				
				if( packet->message->command_response().has_operation_list() )
                    showOperations( packet->message->command_response().operation_list() );
				
				if( packet->message->command_response().has_job_list() )
					showJobs( packet->message->command_response().job_list() );
                
			}
				break;
				
			default:
				txt << "Unknwn packet received\n";

                LM_X(1, ("Unknown packet received at delilahConsole"));
				break;
		}
		
		
		return 0;
	}	
    
    void DelilahConsole::delilahComponentFinishNotification( DelilahComponent *component )
    {
        std::ostringstream o;
        o << "Finished notification " << component->getDescription();

        if( component->error.isActivated() )
            writeErrorOnConsole(o.str());        
        else
            writeWarningOnConsole(o.str());        
            
    }
    
	void DelilahConsole::showQueues( const network::QueueList ql)
	{
		std::ostringstream txt;
		
		txt << "------------------------------------------------------------------------------------------------" << std::endl;
		txt << "Queues" << std::endl;
		txt << "------------------------------------------------------------------------------------------------" << std::endl;
		for (int i = 0 ; i < ql.queue_size() ; i++)
		{
			network::Queue queue = ql.queue(i).queue();
			
			txt << std::setw(30) << queue.name();
			txt << " ";
			txt << au::str( queue.info().kvs() );
			txt << " kvs in ";
			txt << au::str( queue.info().size() ) << " bytes";
			txt << " #File: " << ql.queue(i).file_size();
			txt << " (" << queue.format().keyformat() << " " << queue.format().valueformat() << ") ";
			txt << std::endl;
		}
		txt << "------------------------------------------------------------------------------------------------" << std::endl;
        
        
        const network::StreamOperationList& ol = ql.stream_operation_list();
        
		for (int i = 0 ; i < ol.operation_size() ; i++)
		{
			network::StreamOperation streamOperation = ol.operation(i);
            txt << getStatus( &streamOperation );
			txt << std::endl;
		}

		txt << "------------------------------------------------------------------------------------------------" << std::endl;
		
		txt << std::endl;
		
		writeOnConsole( txt.str() );
		
	}
	
	void DelilahConsole::showDatas( const network::DataList dl)
	{
		std::ostringstream txt;
		
		txt << "------------------------------------------------------------------------------------------------" << std::endl;
		txt << "Datas" << std::endl;
		txt << "------------------------------------------------------------------------------------------------" << std::endl;
		for (int i = 0 ; i < dl.data_size() ; i++)
		{
			network::Data data = dl.data(i);
			txt << std::setw(20) << data.name() << " " << data.help() << std::endl;
		}
		txt << "------------------------------------------------------------------------------------------------" << std::endl;
		txt << std::endl;
		
		writeOnConsole( txt.str() );
		
	}
	void DelilahConsole::showOperations( const network::OperationList ol)
	{
		std::ostringstream txt;
		
		txt << "------------------------------------------------------------------------------------------------" << std::endl;
		txt << "Operations" << std::endl;
		txt << "------------------------------------------------------------------------------------------------" << std::endl;
		for (int i = 0 ; i < ol.operation_size() ; i++)
		{
			network::Operation operation = ol.operation(i);
			txt << "** " << operation.name() << " ( " << operation.type() << " )";
			txt << "\n\t\tInputs: ";
			for (int i = 0 ; i < operation.input_size() ; i++)
				txt << "[" << operation.input(i).keyformat() << "-" << operation.input(i).valueformat() << "] ";
			txt << "\n\t\tOutputs: ";
			for (int i = 0 ; i < operation.output_size() ; i++)
				txt << "[" << operation.output(i).keyformat() << "-" << operation.output(i).valueformat() << "] ";
			
			txt << "\n\t\tHelp: " << operation.help_line() << std::endl;
			txt << "\n";
		}
		txt << "------------------------------------------------------------------------------------------------" << std::endl;
		txt << std::endl;
		
		writeOnConsole( txt.str() );
		
	}
	void DelilahConsole::showJobs( const network::JobList jl)
	{
		std::ostringstream txt;
		txt << "------------------------------------------------------------------------------------------------" << std::endl;
		txt << "Jobs" << std::endl;
		txt << "------------------------------------------------------------------------------------------------" << std::endl;
		for (int i = 0 ; i < jl.job_size() ; i++)
		{
			
			const network::Job job = jl.job(i);
			
			txt << setiosflags(std::ios::right);
			//txt << std::setw(10) << jl.job(i).id();
			//txt << " ";
			txt << std::setw(10) << jl.job(i).status();
			txt << " ";
			txt << jl.job(i).main_command();
			txt << "\n";

			for (int item = 0 ; item < job.item_size() ; item++)
			{
				txt << std::setw(10 + item*2) << " ";
				
                if( job.item(item).has_controller_task() )
                {
                    
                    const network::ControllerTask &task =  job.item(item).controller_task();
                    
                    txt << "[ Task " << task.task_id() << " ] ";
                    
                    switch (task.state()) {
                        case network::ControllerTask_ControllerTaskState_ControllerTaskInit:
                            txt << "Init";
                            break;
                        case network::ControllerTask_ControllerTaskState_ControllerTaskCompleted:
                            txt << "Completed";
                            break;
                        case network::ControllerTask_ControllerTaskState_ControllerTaskFinish:
                            txt << "Finished";
                            break;
                        case network::ControllerTask_ControllerTaskState_ControllerTaskRunning:
                            txt << "Running";
                            
                            double running_progress;
                            if( task.total_info().size() == 0 )
                                running_progress = 0;
                            else
                                running_progress  =  (double) task.running_info().size() / (double) task.total_info().size();
                            
                            double processed_completed;
                            if( task.processed_info().size() == 0)
                                processed_completed = 0;
                            else
                                processed_completed = (double) task.processed_info().size() / (double) task.total_info().size();
                            
                            txt << " " << task.task_description();
                            
                            txt << "\n" << std::setw(10 + item*2) << " ";
                            
                            txt << "\tProgress: ";
                            txt << au::str( task.processed_info().size() );
                            txt << " / " << au::str( task.running_info().size() );
                            txt << "/" << au::str( task.total_info().size() ) << " ";
                            txt << au::Format::double_progress_bar(processed_completed, running_progress, '*', '-', ' ' ,  60);
                            break;
                            
                    }
                    
                    if( task.has_error() )
                        txt << "Error: (" << task.error().message() <<  ")";
                    
                    txt << "\n";                    
                    
                }
                else
                {
                
                    /*
                    txt << "[";
                    txt << au::Format::int_string( job.item(item).line() , 4);
                    txt << "/";
                    txt << au::Format::int_string( job.item(item).num_lines() , 4) ;
                    txt << "] ";
                    */
                     
                    txt << job.item(item).command();
                }
				
				txt << "\n";
				//txt << au::Format::int_string( jl.job(i).progress()*100 , 2) << "%";
			}
			
		}
		txt << "------------------------------------------------------------------------------------------------" << std::endl;
		txt << std::endl;
		
		writeOnConsole( txt.str() );
		
	}
    
    void DelilahConsole::showInfo( std::string command )
    {
        if ( !samsonStatus )
        {
            // If samsonStatus is not received, show a message informing about this
            writeWarningOnConsole("Worker status still not received from SAMSON platform");
            return;
        }
        
        // Lock the info vector to avoid other thread access this information
        au::TokenTaker tt( &info_lock );
        
        // Common string buffer to accumulate the output of the info message        
        std::ostringstream txt;


        txt << "================================================================================================" << std::endl;
        txt << "SAMSON STATUS" << std::endl;
        txt << "================================================================================================" << std::endl;
        
        txt << "------------------------------------------------------------------------------------------------" << std::endl;
        txt << "Controller      ( uptime: " << au::Format::time_string( samsonStatus->controller_status().up_time() ) << " )";
        txt << " ( updated: " << cronometer_samsonStatus.str() <<  " )" <<  std::endl;
        txt << "------------------------------------------------------------------------------------------------" << std::endl;
        
        
        if( (command == "info_modules" ) )
            txt << "** Modules:\n" << samsonStatus->controller_status().modules_manager_status() << "\n";

        if( ( command == "info_full" ) || ( command == "info_setup" ))
            txt << samsonStatus->controller_status().setup_status() << "\n";
        
        if( ( command == "info_full" ) || ( command == "info_task_manager" ))
        {
            txt << "\tJobManager: " << samsonStatus->controller_status().job_manager_status() << std::endl;
            txt << "\tTaskManager: " << std::endl;
            
            for ( int i = 0 ; i < samsonStatus->controller_status().task_manager_status().task_size() ; i++)
            {
                const network::ControllerTask &task =  samsonStatus->controller_status().task_manager_status().task(i);
                
                txt << "\t\t" << "[" << task.task_id() << " / Job: " << task.job_id() << " ] ";
 
                switch (task.state()) {
                    case network::ControllerTask_ControllerTaskState_ControllerTaskInit:
                        txt << "Init";
                        break;
                    case network::ControllerTask_ControllerTaskState_ControllerTaskCompleted:
                        txt << "Completed";
                        break;
                    case network::ControllerTask_ControllerTaskState_ControllerTaskFinish:
                        txt << "Finished";
                        break;
                    case network::ControllerTask_ControllerTaskState_ControllerTaskRunning:
                        txt << "Running";
                        
                        double running_progress;
                        if( task.total_info().size() == 0 )
                            running_progress = 0;
                        else
                            running_progress  =  (double) task.running_info().size() / (double) task.total_info().size();
                        
                        double processed_completed;
                        if( task.processed_info().size() == 0)
                            processed_completed = 0;
                        else
                            processed_completed = (double) task.processed_info().size() / (double) task.total_info().size();
                        
                        txt << "\n\t\tProgress: " << task.task_description() << " : ";
                        txt << au::str( task.processed_info().size() );
                        txt << " / " << au::str( task.running_info().size() );
                        txt << "/" << au::str( task.total_info().size() ) << " ";
                        txt << au::Format::double_progress_bar(processed_completed, running_progress, '*', '-', ' ' ,  60);
                        break;
                        
                }
                
                if( task.has_error() )
                    txt << "Error: (" << task.error().message() <<  ")";
                   
                txt << "\n";
                
                   
                
                //samsonStatus->controller_status().task_manager_status();
            }
                 
            
            txt << std::endl;
        }
        
        if ( command == "info_net" ) 
        {
            txt << samsonStatus->controller_status().network_status() << "\n";
            txt << std::endl;
        }
        
        txt << std::endl;
        
        
        for (int i = 0 ; i < samsonStatus->worker_status_size() ; i++)
        {
            const network::WorkerStatus worker_status = samsonStatus->worker_status(i);
            
            int used_cores = worker_status.used_cores();
            int total_cores = worker_status.total_cores();
            double per_cores = (total_cores==0)?0:((double) used_cores / (double) total_cores);
            size_t used_memory = worker_status.used_memory();
            size_t total_memory = worker_status.total_memory();
            double per_memory = (total_memory==0)?0:((double) used_memory / (double) total_memory);
            int disk_pending_operations = worker_status.disk_pending_operations();
            double per_disk = (total_memory==0)?0:((double) disk_pending_operations / (double) 40);
            
            txt << "------------------------------------------------------------------------------------------------" << std::endl;
            txt << "Worker " << i;
            txt << "  Process: " << au::Format::percentage_string(per_cores).c_str();
            txt << " Memory: " << au::Format::percentage_string(per_memory);
            txt << " Disk: " << disk_pending_operations;
            txt << "  ( uptime: " << au::Format::time_string( worker_status.up_time() ) << " )";
            txt << " ( updated: " << au::Format::time_string( cronometer_samsonStatus.diffTimeInSeconds() + worker_status.update_time() ) << " )" << std::endl;
            txt << "------------------------------------------------------------------------------------------------" << std::endl;

                        
            if( ( command == "info_full" ) || (command == "info_memory_manager" ) )
            {
                txt << "\tMemory Manager:    " << worker_status.memory_status() << "\n";
                txt << "\tShared Memory Manager:    " << worker_status.shared_memory_status() << "\n";
            }

            if( ( command == "info_full" ) || ( command == "info_setup" ))
                txt << worker_status.setup_status() << "\n";
            
            
            if( (command == "info_modules" ) )
                txt << "** Modules:\n" << worker_status.modules_manager_status() << "\n";
            
            if( ( command == "info_full" ) || (command == "info_load_data_manager" ) )
                txt << "** Load Data Manager: " << worker_status.load_data_manager_status() << "\n";

            if( ( command == "info_full" ) || (command == "info_queues" ) )
            {
                txt << "** Queues info:\n" << au::indent( worker_status.queues_status() ) << "\n";
                //txt << "** QueuesTasks: " << worker_status.queues_tasks_status() << "\n";
            }
            
            if( ( command == "info_full" ) || (command == "info_process_manager" ) )
                txt << "** Process Manager:   " << worker_status.process_manager_status() << "\n";
            
            if( ( command == "info_full" ) || (command == "info_task_manager" ) )
                txt << "** Task Manager:      " << worker_status.task_manager_status() << "\n";
            
            if( ( command == "info_full" ) || (command == "info_disk_manager" ) )
                txt << "** Disk Manager:      " << worker_status.disk_manager_status() << "\n";
            if( ( command == "info_full" ) || (command == "info_engine" ) )
                txt << "\tEngine:      " << worker_status.engine_status() << "\n";
            
            if ( command == "info_net" ) 
                txt << worker_status.network_status() << "\n";
            
            
            if( command == "info_cores" )
            {
                txt << au::str("Worker %03d", i);
                
                
                txt << au::str("\n\tCores  [ %s ] %s / %s :" , 
                                          au::Format::percentage_string(per_cores).c_str() , 
                                          au::str(used_cores).c_str() , 
                                          au::str(total_cores).c_str() );
                
                txt << au::Format::progress_bar( per_cores , 50 );
                
                
                txt << au::str("\n\tMemory [ %s ] %s / %s :" , 
                                          au::Format::percentage_string(per_memory).c_str() , 
                                          au::str(used_memory).c_str() , 
                                          au::str(total_memory).c_str() );
                
                txt << au::Format::progress_bar( per_memory , 50 );
                
                // Disk operations
                
                txt << au::str("\n\tDisk                     %s :" , 
                                          au::str(disk_pending_operations).c_str() );
                
                txt << au::Format::progress_bar( per_disk , 50 );
				txt << "\n";                    
            }
        }
        
        txt << "================================================================================================" << std::endl;
        txt << std::endl;
        
        
        // Send to the console screen
        writeOnConsole( txt.str() );
        
    }
    
    bool DelilahConsole::checkXMLInfoUpdate()
    {
        int soft_limit = 10;
        int hard_limit  = 60;
        
        int time = getUpdateSeconds();
        
        if( time < soft_limit )
            return true;

        if( time < hard_limit )
        {
            writeWarningOnConsole( au::str( "Monitorization information is %d seconds old" , time ) );
            return true;
        }
        
        writeErrorOnConsole( au::str( "Monitorization information is %d seconds old" , time ) );
        return false;
    }
    
}
