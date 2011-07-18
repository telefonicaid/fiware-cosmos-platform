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
            
			au::CommandLine cmdLine;
			cmdLine.parse( rl_line_buffer );
			std::string mainCommand = cmdLine.get_argument(0);
            
			//std::cout << "Completion for " << mainCommand << " \n";
			
			if( ( mainCommand == "clear" ) || ( mainCommand == "rm" ) || ( mainCommand == "cp" ) || ( mainCommand == "mv" ) )
            {
                // Add all the queues
                op.addQueueOptions( NULL );
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
        
        char ** matches = op.get(text);

        /*
        if( matches )
        {
            int pos = 0;
            while( matches[pos] != NULL)
            {
                std::cerr << "Options " << matches[pos] << "\n";
                pos++;
            }
        }
         */
        
        return matches;
        
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
    
    
	size_t DelilahConsole::runAsyncCommand( std::string command )
	{
		
		au::CommandLine commandLine;
		commandLine.set_flag_string("name", "null");
		commandLine.set_flag_string("begin", "null");
		commandLine.set_flag_boolean("show");
		commandLine.set_flag_boolean("plain");
		commandLine.set_flag_boolean("gz");			// Flag to indicate compression
		commandLine.set_flag_int("threads",4);
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
			
			std::ostringstream output;

			output << "SAMSON v " << SAMSON_VERSION << "\n";
			output << "==============================================================\n";
                        output << "\n";
                        output << " ls          Get a list of current data-sets\n";
                        output << "             Usage: ls [-begin name] [-end name]\n";
                        output << "\n";
                        output << " add         Add a data set with a particular format for the key and for the value\n";
                        output << "             Usage: add set_name key_type value_type (i.e. add pairs system.UInt system.UInt )\n";
                        output << "             Usage: add set_name -txt (i.e. add txt_cdrs )\n";
                        output << "             Option -f to avoid error if the data set already exist\n";
                        output << "\n";
                        output << " rm          Remove a given data set\n";
                        output << "             Usage: rm set_name\n";
                        output << "\n";
                        output << " mv          Change the name of a particular set\n";
                        output << "             Usage: mv set_name set_name2\n";
                        output << "\n";
                        output << " clear       Clear the content of a particular data set\n";
                        output << "             Usage: clear set_name\n";
                        output << "\n";
                        output << "---------------------------------------------------------------------\n";
                        output << "\n";
                        output << " set/unset   Set and unset environment variables that go to all operations\n";
                        output << "             Usage: set var_name value or unset var_name\n";
                        output << "\n";
                        output << "---------------------------------------------------------------------\n";
                        output << "\n";
                        output << " operations  Get a list of the available operations ( parser, map, reduce, script, etc)\n";
                        output << "             Usage: operations/o [-begin X] [-end -X]\n";
                        output << "\n";
                        output << " datas       Get a list of available data types for keys and values\n";
                        output << "             Usage: datas/d [-begin X] [-end -X]\n";
                        output << "\n";
                        output << "---------------------------------------------------------------------\n";
                        output << "\n";
                        output << " jobs (j)    Get a list of running jobs\n";
                        output << "             Usage: jobs/j\n";
                        output << " clear_jobs (cj)    Clear finish or error jobs\n";
                        output << "             Usage: clear_jobs/cj\n";
                        output << " kill (k)    Kill a particular job and all its sub-tasks\n";
                        output << "             Usage: kill job_id\n";
                        output << "\n";
                        output << " workers (w) Get information about what is running on workers and controller\n";
                        output << "             Usage: workers/w\n";
                        output << "\n";
                        output << " reload_modules Reload modules present used by the platform.\n";
                        output << "             Usage: reload_modules/reload\n";
                        output << "\n";
                        output << "---------------------------------------------------------------------\n";
                        output << "\n";
                        output << " upload      Load txt files to the platform\n";
                        output << "             Usage: load local_file_name set_name [-plain] [-gz] \n";
                        output << "\n";
                        output << " download    Download txt files from the platform\n";
                        output << "             Usage: download set_name local_file_name (only txt txt at the moment)/w\n";
                        output << "\n";
                        output << " load        Check status of upload and downloads \n";
                        output << "             Usage: load\n";
                        output << "\n";
                        output << " load_clear  Clear complete upload and download processes\n";
                        output << "             Usage: load_clear\n";
                        output << "\n";
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
			output << "\n";
			
			
			writeOnConsole( output.str() );
			return 0;
		}
		
		if ( commandLine.isArgumentValue(0, "quit", "") )
		{
			Console::quitConsole();	// Quit the console
			return 0;
		}

		
        if ( mainCommand == "e" )
        {
            // Experimental
            
            if( commandLine.get_num_arguments() == 1)
                return 0;
            
            if( commandLine.get_argument(1) == "show" )
                writeOnConsole( stream::BlockManager::shared()->str() );

            if( commandLine.get_argument(1) == "set" )
            {
                if ( commandLine.get_num_arguments() < 4 )
                {
                    writeOnConsole("Not enougth parameters. e set (id) (priority)");
                    return 0;
                }
                
                int id = atoi( commandLine.get_argument(2).c_str() );
                int priority = atoi( commandLine.get_argument(3).c_str() );
                
                stream::Block*b = stream::BlockManager::shared()->getBlock(id);
                if( b )
                {
                    b->setPriority(priority);
                    writeOnConsole( au::Format::string("Seting priority %d to id %d", priority , id ) );
                }
                else
                {
                    writeOnConsole( au::Format::string("Block %d not found", id ) );
                }
                
            }
            
            if( commandLine.get_argument(1) == "retain" )
            {
                if ( commandLine.get_num_arguments() < 4 )
                {
                    writeOnConsole("Not enougth parameters. e retain (id) (task_id) ");
                    return 0;
                }
                
                int id = atoi( commandLine.get_argument(2).c_str() );
                int task_id = atoi( commandLine.get_argument(3).c_str() );
                
                stream::Block*b = stream::BlockManager::shared()->getBlock(id);
                if( b )
                {
                    stream::BlockManager::shared()->retain( b , task_id );
                    writeOnConsole( au::Format::string("Retained %d by task %d", id , task_id ) );
                }
                else
                {
                    writeOnConsole( au::Format::string("Block %d not found", id ) );
                }
                
            }

            if( commandLine.get_argument(1) == "release" )
            {
                if ( commandLine.get_num_arguments() < 4 )
                {
                    writeOnConsole("Not enougth parameters. e release (id) (task_id) ");
                    return 0;
                }
                
                int id = atoi( commandLine.get_argument(2).c_str() );
                int task_id = atoi( commandLine.get_argument(3).c_str() );
                
                stream::Block*b = stream::BlockManager::shared()->getBlock(id);
                if( b )
                {
                    stream::BlockManager::shared()->release( b , task_id );
                    writeOnConsole( au::Format::string("Released %d by task %d", id , task_id ) );
                }
                else
                {
                    writeOnConsole( au::Format::string("Block %d not found", id ) );
                }
                
            }
            
            if( commandLine.get_argument(1) == "add" )
            {
                                
                engine::Buffer *buffer = engine::MemoryManager::shared()->newBuffer("example", 100000000, 0 );
                buffer->setSize( buffer->getMaxSize() );    // Full the buffer with crap content ;)
                stream::Block *block = new stream::Block( buffer , true );

                // Change priority
                int new_priority = rand()%10;
                writeOnConsole( au::Format::string("Setting priority to  %d", new_priority ) );
                block->setPriority(new_priority);

            }
            
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
		
		if ( mainCommand == "load_clear" )
		{
			// Clear completed upload and download process
			clearComponents();
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

			size_t id = addDownloadProcess(queue_name, fileName , commandLine.get_flag_bool("show"));

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
            
            
            std::istringstream is_xml_document( xml_info );
            pugi::xml_document doc;
            pugi::xml_parse_result result = doc.load( is_xml_document );
            
            if (!result)
            {
                writeErrorOnConsole( au::Format::string("Error parsing xml from samson platform %s", result.description() ) );
                return 0;
            }
            
            
            std::string command = commandLine.get_argument(1);
            
            if( command == "xml" )
            {
                writeOnConsole( pugi::str( doc ) );
                return 0;
            }
            
            if ( command == "txt" )
            {
                writeOnConsole(  xml_info );
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
                writeWarningOnConsole(au::Format::string("Running select %s" , query.c_str()));
                
                pugi::xpath_node_set result;
                try {
                    result = doc.select_nodes( query.c_str() );
                } catch (pugi::xpath_exception& ex) 
                {
                    writeErrorOnConsole( au::Format::string( "Error in xpath query: %s" , ex.what() ) );
                    return 0;
                }
                
                // Transform the results into a string
                std::ostringstream result_txt;
                pugi::str( result , result_txt );
                
                writeOnConsole(result_txt.str());
                
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
                writeWarningOnConsole(au::Format::string("Running select %s" , query.c_str()));
                
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

                writeWarningOnConsole(au::Format::string("Running query %s wiht result %lu" , query_str.c_str() , result ) );
                
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
                
                writeWarningOnConsole(au::Format::string("Running query %s wiht result %f" , query_str.c_str() , result ) );
                
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
                
                writeWarningOnConsole(au::Format::string("Running query %s wiht result %s" , query_str.c_str() , result.c_str() ) );
                
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
				writeErrorOnConsole("Usage: push file <file2> .... <fileN> queue");
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
            
			size_t id = addPushData(fileNames, queue );
			
			std::ostringstream o;
			o << "[ " << id << " ] Push data process started with " << fileNames.size() << " files";
			writeWarningOnConsole(o.str());
			return id;
		}
        

        // Push data to a queue
        
        if( mainCommand == "pop" )
		{
			if( commandLine.get_num_arguments() < 5 )
			{
				writeErrorOnConsole("Usage: pop queue channel parserOut fileName");
				return 0;
			}
            
            std::string queue_name = commandLine.get_argument(1);
            int channel = atoi( commandLine.get_argument(2).c_str() );
            std::string parserOut = commandLine.get_argument(3);
            std::string fileName = commandLine.get_argument(4);
            
			size_t id = addPopData( queue_name , channel , parserOut , fileName );
			
			std::ostringstream o;
			o << "[ " << id << " ] Pop from queue " << queue_name << ":" << channel << " started. ParserOut " << parserOut << " LocalFile: " << fileName;
			writeWarningOnConsole(o.str());
			return id;
		}
        
        
		// Normal command send to the controller
		return sendCommand( command , NULL );

		//std::ostringstream o;
		//o << "Sent command to controller (id="<<id<<") : " << command;
		//writeWarningOnConsole(o.str());
		
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
                
                //writeOnConsole( au::Format::string("Trace: %s", _text.c_str() ) );
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
	
	void DelilahConsole::uploadDataConfirmation( DelilahUploadDataProcess *process)
	{
		if( process->error.isActivated() )
		{
			std::ostringstream o;
			o << "[ " << process->id << " ] Upload data process finished with error ("<< process->error.getMessage() << "). Type XXX for more information";
			writeErrorOnConsole(o.str());
		}
		else
		{
			std::ostringstream o;
			o << "[ " << process->id << " ] Upload data process finished correctly\n";
			writeWarningOnConsole(o.str());
		}
	};
	
	void DelilahConsole::downloadDataConfirmation( DelilahDownloadDataProcess *process)
	{
		if( process->error.isActivated() )
		{
			std::ostringstream o;
			o << "[ " << process->id << " ] Download data process finished with error ( "<< process->getDescription() << " )\n";
			o << "\tERROR: " << process->error.getMessage();
			writeErrorOnConsole(o.str());
		}
		else
		{
			std::ostringstream o;
			o << "[ " << process->id << " ] Download data process finished correctly ( "<< process->getDescription() << " )";
			writeWarningOnConsole(o.str());
		}
	};
    
    void DelilahConsole::pushConfirmation( PushComponent *process )
    {
		if( process->error.isActivated() )
		{
			std::ostringstream o;
			o << "[ " << process->id << " ] Push data process finished with error ( " << process->getStatus() << " )\n";
			o << "\tERROR: " << process->error.getMessage();
			writeErrorOnConsole(o.str());
		}
		else
		{
			std::ostringstream o;
			o << "[ " << process->id << " ] Push data process finished correctly ( " << process->getStatus() << " )";
			writeWarningOnConsole(o.str());
		}
        
    }

    void DelilahConsole::popConfirmation( PopComponent *process )
    {
		if( process->error.isActivated() )
		{
			std::ostringstream o;
			o << "[ " << process->id << " ] Pop process finished with error ( " << process->getStatus() << " )\n";
			o << "\tERROR: " << process->error.getMessage();
			writeErrorOnConsole(o.str());
            
		}
		else
		{
			std::ostringstream o;
			o << "[ " << process->id << " ] Pop  process finished correctly ( " << process->getStatus() << " )";
			writeWarningOnConsole(o.str());
            
            size_t file_size = au::Format::sizeOfFile( process->fileName );
            
            if( file_size < 10000 )
            {
                writeOnConsole("Content: --------------------------------------------------------------");
                FILE *file = fopen( process->fileName.c_str() , "r" );
                char * buffer = (char*) malloc( file_size );
                int nb = fread( buffer , file_size , 1 , file );
		if (nb == 0)
		{
			writeWarningOnConsole("fread reads 0 instead of " + au::Format::string(file_size));	
		}
                
                writeOnConsole( buffer );
                
                writeOnConsole("End of Content: -------------------------------------------------------");
                free( buffer );
            }
            else
                writeWarningOnConsole("Not showing on screen since file is too large ( " + au::Format::string(file_size) + " )" );
            
		}
        
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
			txt << au::Format::string( queue.info().kvs() );
			txt << " kvs in ";
			txt << au::Format::string( queue.info().size() ) << " bytes";
			txt << " #File: " << ql.queue(i).file_size();
			txt << " (" << queue.format().keyformat() << " " << queue.format().valueformat() << ") ";
			txt << std::endl;
		}
		txt << "------------------------------------------------------------------------------------------------" << std::endl;
        
		for (int i = 0 ; i < ql.stream_queue_size() ; i++)
		{
			network::StreamQueue queue = ql.stream_queue(i);
            txt << getStatus( &queue );
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
			txt << "** " << operation.name();
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
                            txt << au::Format::string( task.processed_info().size() );
                            txt << " / " << au::Format::string( task.running_info().size() );
                            txt << "/" << au::Format::string( task.total_info().size() ) << " ";
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
                        txt << au::Format::string( task.processed_info().size() );
                        txt << " / " << au::Format::string( task.running_info().size() );
                        txt << "/" << au::Format::string( task.total_info().size() ) << " ";
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
                txt << "** Queues info:\n" << au::Format::indent( worker_status.queues_status() ) << "\n";
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
                txt << au::Format::string("Worker %03d", i);
                
                
                txt << au::Format::string("\n\tCores  [ %s ] %s / %s :" , 
                                          au::Format::percentage_string(per_cores).c_str() , 
                                          au::Format::string(used_cores).c_str() , 
                                          au::Format::string(total_cores).c_str() );
                
                txt << au::Format::progress_bar( per_cores , 50 );
                
                
                txt << au::Format::string("\n\tMemory [ %s ] %s / %s :" , 
                                          au::Format::percentage_string(per_memory).c_str() , 
                                          au::Format::string(used_memory).c_str() , 
                                          au::Format::string(total_memory).c_str() );
                
                txt << au::Format::progress_bar( per_memory , 50 );
                
                // Disk operations
                
                txt << au::Format::string("\n\tDisk                     %s :" , 
                                          au::Format::string(disk_pending_operations).c_str() );
                
                txt << au::Format::progress_bar( per_disk , 50 );
				txt << "\n";                    
            }
        }
        
        txt << "================================================================================================" << std::endl;
        txt << std::endl;
        
        
        // Send to the console screen
        writeOnConsole( txt.str() );
        
    }
}
