/* ****************************************************************************
*
* FILE                     DelilahConsole.h
*
* DESCRIPTION			   Console terminal for delilah
*
*/
#include "Delilah.h"					// ss::Delailh
#include "DelilahConsole.h"				// Own interface
#include "Packet.h"						// ss:Packet
#include "au/Format.h"						// au::Format
#include "DelilahUploadDataProcess.h"		// ss::DelilahUpLoadDataProcess
#include "DelilahDownloadDataProcess.h"		// ss::DelilahDownLoadDataProcess
#include "engine/MemoryManager.h"				// ss::MemoryManager
#include <iostream>
#include <iomanip>
#include "EnvironmentOperations.h"						// Environment operations (CopyFrom)
#include <iomanip>
#include "samson/samsonVersion.h"		// SAMSON_VERSION
#include <sys/stat.h>					// stat(.)
#include <dirent.h>						// DIR directory header	
#include "au/CommandLine.h"				// au::CommandLine

namespace ss
{	
	// Information about all queues and operations	
	
	
	AutoCompletionOptions completion_options;
	
	char * dupstr (const char *s)
	{
		char *r;
		
		r = (char*)malloc (strlen (s) + 1);
		strcpy (r, s);
		return (r);
	}
	
	
	void addMainCommandOptions()
	{
		completion_options.clearOptions();
		
		completion_options.addOption("ls");
		completion_options.addOption("mv");
		completion_options.addOption("add");
		completion_options.addOption("operations");
		completion_options.addOption("datas");
		completion_options.addOption("jobs");
		completion_options.addOption("workers");
		completion_options.addOption("upload");
		completion_options.addOption("download");
		completion_options.addOption("load");
		completion_options.addOption("clear");
		completion_options.addOption("help");
		completion_options.addOption("set");
		completion_options.addOption("unset");
        completion_options.addOption("info");
        completion_options.addOption("info_full");
        completion_options.addOption("info_net");
        completion_options.addOption("info_cores");
        completion_options.addOption("info_task_manager");
        completion_options.addOption("info_disk_manager");
        completion_options.addOption("info_process_manager");
        completion_options.addOption("info_memory_manager");
        completion_options.addOption("info_load_data_manager");
        completion_options.addOption("info_engine");

        
		// add available operations...
		info_lock.lock();
		
		if( ol )
			for (int i = 0 ; i < ol->operation_size()  ; i++)
				completion_options.addOption( ol->operation(i).name() );
		
		info_lock.unlock();
		
	}
	
	
	void addQueueOptions( network::KVFormat *format )
	{
		
		completion_options.clearOptions();
		
		// add available queues...
		info_lock.lock();
				
		if( ql )
			for (int i = 0 ; i < ql->queue_size()  ; i++)
			{
				
				const network::Queue &queue = ql->queue(i).queue();
				
				//std::cout << "Considering " << queue.name() << "\n";
				
				if( !format )
					completion_options.addOption( queue.name() );
				else
				{
					//std::cout << "Checkling formats "  << queue.format().keyformat() << " " << queue.format().valueformat()<<   "\n";
					if( ( queue.format().keyformat() == format->keyformat() ) )
						if ( queue.format().valueformat() == format->valueformat() )
						{
							completion_options.addOption( ql->queue(i).queue().name() );
							//std::cout << "added\n";
						}
					
				}
			}
		
		info_lock.unlock();
		
	}
	
	
	/* Generator function for command completion.  STATE lets us know whether
	 to start from scratch; without any state (i.e. STATE == 0), then we
	 start at the top of the list. */
	
	char * command_generator (const char * text, int state)
	{
		//std::cout << "Command generator with \"" << text << "\" and state=" << state << "\n";

#if 0
		static int list_index;
		static int operation_index;
		char *name;
		
		/* If this is a new word to complete, initialize now.  This includes
		 saving the length of TEXT for efficiency, and initializing the index
		 variable to 0. */
		if (!state)
		{
			list_index = 0;
			operation_index = 0;
		}
		
		int len = strlen (text);

		
		/* Return the next name which partially matches from the command list. */
		while ((name = (char*)commands[list_index]))
		{
			list_index++;
			if (strncmp (name, text, len) == 0)
				return (dupstr(name));
		}
		
		/* If no names matched, then return NULL. */
		return ((char *)NULL);
#endif
		
		if( !state )
			completion_options.clearSearch();
		
		return completion_options.next(text);
		
	}	
	
	char ** readline_completion ( const char* text, int start,int end )
	{
		//std::cout << "Readline completion with \"" << text << "\" S=" << start << " E=" << end <<" \n";
		
		char **matches;
		
		matches = (char **)NULL;
		
		
		if (start == 0)
		{
			addMainCommandOptions();
			//std::cout << "Completion for first command \n";
		}
		else
		{
			au::CommandLine cmdLine;
			cmdLine.parse(rl_line_buffer);
			std::string mainCommand = cmdLine.get_argument(0);

			//std::cout << "Completion for " << mainCommand << " \n";
			
			if( ( mainCommand == "clear" ) || ( mainCommand == "rm" ) || ( mainCommand == "cp" ) || ( mainCommand == "mv" ) )
				addQueueOptions( NULL );
			else
			{
				
				network::KVFormat *format = NULL;
				int argument_pos = cmdLine.get_num_arguments() - 1;
				if ( ( argument_pos > 0 )  && (rl_line_buffer[strlen(rl_line_buffer)-1]!=' '))
					argument_pos--;	// Still in this parameter
				
				//std::cout << "Argument " << argument_pos << " \n";

				
				// If it is a particular operation... lock for the rigth queue
				info_lock.lock();
				
				
				if( ol )
					for (int i = 0 ; i < ol->operation_size() ; i++)
						if( ol->operation(i).name() == mainCommand )
						{
							//std::cout << "op found " << ol->operation(i).input_size() << "/" << ol->operation(i).output_size() <<  " ("<< argument_pos << ")\n";
							
							if( argument_pos < ol->operation(i).input_size() )
							{
								format = new network::KVFormat();
								format->CopyFrom( ol->operation(i).input(argument_pos) );
							}
							else
							{
								argument_pos -= ol->operation(i).input_size();
								if( argument_pos < ol->operation(i).output_size() )
								{
									format = new network::KVFormat();
									format->CopyFrom( ol->operation(i).output(argument_pos) );
								}
							}
							break; // No more for...
						}
				
				info_lock.unlock();

				
				// Use format if any
				if( format )
				{
					//std::cout << "Format found " << format->keyformat() << " " << format->valueformat() << "\n";
					addQueueOptions(format);
					delete format;
				}
				else
					completion_options.clearOptions();

				
				
			}
		}
		
		matches = rl_completion_matches (text, command_generator);
		return (matches);
	}	
	
	void DelilahConsole::evalCommand( std::string command )
	{
		runAsyncCommand( command );
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
			clear();
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
			output << "             Usage: mr set_name set_name2\n";
			output << "\n";
			output << " clear       Clear the content of a particular data set\n";
			output << "             Usage: clear set_name\n";
			output << "\n";
			output << "---------------------------------------------------------------------\n";
			output << "\n";
			output << " set/unset   Set and unset environment variables that go to all operations\n";
			output << "             Usage: set var_name value or unser var_name\n";
			output << "\n";
			output << "---------------------------------------------------------------------\n";
			output << "\n";
			output << " operations  Get a list of the available operations ( parser, map, reduce, script, etc)\n";
			output << "             Usage: operations/o [-begin X] [-end -X]\n";
			output << "\n";
			output << " datas       Get a list of avilable data types for keys and values\n";
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
			output << " upload		Load txt files to the platform\n";
			output << "             Usage: load local_file_name set_name [-plain] [-gz] \n";
			output << "\n";
			output << " download    Download txt files from the platform\n";
			output << "             Usage: download set_name local_file_name (only txt txt at the moment)/w\n";
			output << "\n";
			output << " load		Check status of upload and downloads \n";
			output << "\n";
			output << " load_clear	Clear complete upload and download processes\n";
			output << "\n";
			output << " load_trace	Activate or deactivate traces for upload and download processes \n";
			output << "             Usage: load_trace on/off\n";
			output << "---------------------------------------------------------------------\n";
			
			output << "\n";
			
			
			writeOnConsole( output.str() );
			return 0;
		}
		
		if ( commandLine.isArgumentValue(0,"quit","") )
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
		
		// Normal command send to the controller
		size_t id = sendCommand(command);
		return id;

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
                if (lmOk(packet->message->trace().type(), packet->message->trace().tlev() ) == LmsOk)
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
                
            }
                break;
                
			case Message::CommandResponse:
			{
				
				if( packet->message->command_response().has_new_job_id() )
				{
					std::ostringstream message;
					message << "Job scheduled [" << packet->message->command_response().new_job_id() << "] ";
					message << " (" << packet->message->command_response().command() << " )";
					writeWarningOnConsole( message.str() );
					return 0;
				}
				
				if( packet->message->command_response().has_finish_job_id() )
				{
					std::ostringstream message;
					message << "Job finished [" << packet->message->command_response().finish_job_id() << "] ";
					message << " [ "<< au::Format::time_string( packet->message->command_response().ellapsed_seconds() ) << " ] ";
					message << " ( " << packet->message->command_response().command() << " )";
					writeWarningOnConsole( message.str() );
					return 0;
				}
	
				if( packet->message->command_response().has_error_job_id() )
				{
					std::ostringstream message;
					message << "Job finished with error [" << packet->message->command_response().error_job_id() << "] ";
					message << " ( " << packet->message->command_response().command() << ")\n\n";
					
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
					cmdLine.parse(packet->message->command_response().command() );
					
					if( cmdLine.get_flag_bool("all") )
					{
						// Copy the list of queues for auto-completion
						info_lock.lock();
						
						if( ql )
							delete ql;
						ql = new network::QueueList();
						ql->CopyFrom( packet->message->command_response().queue_list() );
						
						info_lock.unlock();
						
					}
					else
					{
						showQueues( packet->message->command_response().queue_list() );
					}
					
				}
				
				if( packet->message->command_response().has_automatic_operation_list() )
					showAutomaticOperations( packet->message->command_response().automatic_operation_list() );
				
				if( packet->message->command_response().has_data_list() )
					showDatas( packet->message->command_response().data_list() );
				
				if( packet->message->command_response().has_operation_list() )
				{
					// Check if it is a -all command
					au::CommandLine cmdLine;
					cmdLine.set_flag_boolean("all");
					cmdLine.parse(packet->message->command_response().command() );
					
					if( cmdLine.get_flag_bool("all") )
					{
						info_lock.lock();
						
						if( ol )
							delete ol;
						ol = new network::OperationList();
						ol->CopyFrom( packet->message->command_response().operation_list() );
						
						info_lock.unlock();
						
					}
					else
					{
						showOperations( packet->message->command_response().operation_list() );
					}					
				}
				
				if( packet->message->command_response().has_job_list() )
					showJobs( packet->message->command_response().job_list() );
                
			}
				break;
				
			default:
				txt << "Unknwn packet received\n";
				exit(1);
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
	
	
	
	void DelilahConsole::showAutomaticOperations( const network::AutomaticOperationList aol)
	{
		std::ostringstream txt;
		
		txt << "------------------------------------------------------------------------------------------------" << std::endl;
		txt << "Automatic Operations" << std::endl;
		txt << "------------------------------------------------------------------------------------------------" << std::endl;
		for (int i = 0 ; i < aol.automatic_operation_size() ; i++)
		{
			network::AutomaticOperation ao = aol.automatic_operation(i);

			txt << std::setw(10) << ao.id();
			txt << " ";
			txt << std::setw(20) << ao.thrigger();
			txt << " ";
			txt << ao.command();
			txt << std::endl;
		}
		txt << "------------------------------------------------------------------------------------------------" << std::endl;
		
		txt << std::endl;
		
		writeOnConsole( txt.str() );		
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
			txt << std::setw(10) << jl.job(i).id();
			txt << " ";
			txt << std::setw(10) << jl.job(i).status();
			txt << " ";
			txt << jl.job(i).main_command();
			txt << "\n";

			for (int item = 0 ; item < job.item_size() ; item++)
			{
				txt << std::setw(10 + item*2) << " ";
				
				txt << "[";
				txt << au::Format::int_string( job.item(item).line() , 4);
				txt << "/";
				txt << au::Format::int_string( job.item(item).num_lines() , 4) ;
				txt << "] ";
				txt << job.item(item).command();
				
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
        info_lock.lock();
        
        // Common string buffer to accumulate the output of the info message        
        std::ostringstream txt;


        txt << "================================================================================================" << std::endl;
        txt << "SAMSON STATUS" << std::endl;
        txt << "================================================================================================" << std::endl;
        
        txt << "------------------------------------------------------------------------------------------------" << std::endl;
        txt << "Controller      ( uptime: " << au::Format::time_string( samsonStatus->controller_status().up_time() ) << " )";
        txt << " ( updated: " << cronometer_samsonStatus.str() <<  " )" <<  std::endl;
        txt << "------------------------------------------------------------------------------------------------" << std::endl;
        
        if( ( command == "info_full" ) || ( command == "info_task_manager" ))
        {
            txt << "\tJobManager: " << samsonStatus->controller_status().job_manager_status() << std::endl;
            txt << "\tTaskManager: " << std::endl;
            
            for ( int i = 0 ; i < samsonStatus->controller_status().task_manager_status().task_size() ; i++)
            {
                const network::ControllerTask &task =  samsonStatus->controller_status().task_manager_status().task(i);
                
                txt << "\t\t" << "[" << task.task_id() << " / Job: " << task.job_id() << " ] ";
 
                switch (task.status()) {
                    case network::ControllerTask_ControllerTaskStatus_ControllerTaskError:
                        txt << "ERROR";
                        break;
                    case network::ControllerTask_ControllerTaskStatus_ControllerTaskCompleted:
                        txt << "Completed";
                        break;
                    case network::ControllerTask_ControllerTaskStatus_ControllerTaskFinish:
                        txt << "Finished";
                        break;
                    case network::ControllerTask_ControllerTaskStatus_ControllerTaskRunning:
                        txt << "Running";
                        
                        double running_progress  = (double) task.running_info().size() / (double) task.total_info().size();
                        double processed_completed = (double) task.processed_info().size() / (double) task.total_info().size();
                        
                        txt << " Progress: ";
                        txt << au::Format::string( task.running_info().size() );
                        txt << " / " << au::Format::string( task.processed_info().size() );
                        txt << "/" << au::Format::string( task.total_info().size() ) << " ";
                        txt << au::Format::double_progress_bar(processed_completed, running_progress, '*', '-', 60);
                        txt << "\n";    
                        break;
                        
                }
                   
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
            
            if( ( command == "info_full" ) || (command == "info_load_data_manager" ) )
                txt << "\tLoad Data Manager: " << worker_status.load_data_manager_status() << "\n";
            
            if( ( command == "info_full" ) || (command == "info_process_manager" ) )
                txt << "\tProcess Manager:   " << worker_status.process_manager_status() << "\n";
            
            if( ( command == "info_full" ) || (command == "info_task_manager" ) )
                txt << "\tTask Manager:      " << worker_status.task_manager_status() << "\n";
            
            if( ( command == "info_full" ) || (command == "info_disk_manager" ) )
                txt << "\tDisk Manager:      " << worker_status.disk_manager_status() << "\n";
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
        
        // Unlock the common information lock    
        info_lock.unlock();

    }
    

    
    
    
    
	
}
