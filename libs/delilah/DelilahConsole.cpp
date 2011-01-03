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
#include "Format.h"						// au::Format
#include "DelilahUploadDataProcess.h"		// ss::DelilahLoadDataProcess
#include "MemoryManager.h"				// ss::MemoryManager
#include <iostream>
#include <iomanip>
#include "EnvironmentOperations.h"						// Environment operations (CopyFrom)
#include <iomanip>
#include "samson/samsonVersion.h"		// SAMSON_VERSION
#include <sys/stat.h>					// stat(.)
#include <dirent.h>						// DIR directory header	
namespace ss
{
	
	void cancel_ncurses(void)	
	{
		au::Console::cancel_ncurses();
	}
	
	void DelilahConsole::evalCommand( std::string command )
	{
		assert( delilah );
		
		au::CommandLine commandLine;
		commandLine.set_flag_string("name", "null");
		commandLine.set_flag_string("begin", "null");
		commandLine.set_flag_boolean("show");
		commandLine.parse( command );

		std::string mainCommand;

		if( commandLine.get_num_arguments() == 0)
		{
			clear();
			return;
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
			output << "             Usage: add set_name key_type value_type (i.e add_queue pairs system.UInt system.UInt )\n";
			output << "             Usage: add set_name -txt (i.e add_queue txt_cdrs )\n";
			output << "             Option -f to avoid error if queue already exist\n";
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
			output << "\n";
			output << " workers (w) Get information about what is running on workers and controller\n";
			output << "             Usage: workers/w\n";
			output << "\n";
			output << "---------------------------------------------------------------------\n";
			output << "\n";
			output << " upload		Load txt files to the platform\n";
			output << "             Usage: load local_file_name set_name (only txt txt at the moment)\n";
			output << "\n";
			output << " download    Download txt files from the platform\n";
			output << "             Usage: download set_name local_file_name (only txt txt at the moment)/w\n";
			output << "\n";
			output << " load		Check status of upload and downloads \n";
			output << "\n";
			output << "---------------------------------------------------------------------\n";
			
			output << "\n";
			
			
			writeBlockOnConsole( output.str() );
			return;
		}
		
		if ( commandLine.isArgumentValue(0,"quit","") )
		{
			exit(0);
			return;
		}
		
		if ( mainCommand == "set")
		{
			if ( commandLine.get_num_arguments() == 1)
			{
				// Only set, we show all the defined parameters
				std::ostringstream output;
				output << "Environent variables:\n";
				output << "------------------------------------\n";
				output << delilah->environment.toString();
				
				writeBlockOnConsole( output.str() );
				return;
			}
			
			if ( commandLine.get_num_arguments() < 3 )
			{
				writeErrorOnConsole("Usage: set name value.");
				return;
			}
			
			// Set a particular value
			std::string name = commandLine.get_argument(1);
			std::string value = commandLine.get_argument(2);
			
			delilah->environment.set( name , value );
			
			return ;
		}

		if ( mainCommand == "unset")
		{
			if ( commandLine.get_num_arguments() < 2 )
			{
				writeErrorOnConsole("Usage: unset name.");
				return;
			}
			
			// Set a particular value
			std::string name = commandLine.get_argument(1);
			
			delilah->environment.unset( name );
			
			return ;
		}		
		
		if ( mainCommand == "download")
		{
			if ( commandLine.get_num_arguments() < 3)
			{
				writeErrorOnConsole( "Error: Usage: download queue local_file_name\n");
				return;
			}
			
			std::string queue_name = commandLine.get_argument(1);
			std::string fileName = commandLine.get_argument(2);

			size_t id = delilah->addDownloadProcess(queue_name, fileName , commandLine.get_flag_bool("show"));

			std::ostringstream o;
			o << "Download data process (id="<<id<<") started.\n";
			writeWarningOnConsole(o.str());
			return;
		}
		
		
		
		if ( mainCommand == "load" )
		{
			
			std::ostringstream output;
			
			MemoryManager::shared()->getStatus( output , "");
			
			
			output << "Load processes....\n";

			std::map<size_t,DelilahComponent*>::iterator iter;
			for (iter = delilah->components.begin() ; iter != delilah->components.end() ; iter++)
				output << iter->second->getStatus();
			
			writeBlockOnConsole(output.str());
			
			
			return ;
			
		}
		
		
		if( mainCommand == "quit" )
		{
			delilah->quit();
			return;
		}

		if (mainCommand == "netstate")
		{
			std::string s;

			s = delilah->network->getState(command);
			writeBlockOnConsole(s);

			return;
		}
		
		if ( mainCommand == "help" )
		{
			
			// Ask for all help
			Packet*         p    = new Packet();
			network::Help*  help = p->message.mutable_help();

			if( commandLine.get_flag_string("name") != "null")
				help->set_name( commandLine.get_flag_string("name") );
			
			if( commandLine.get_flag_string("begin") != "null")
				help->set_begin( commandLine.get_flag_string("begin") );
			
			
			if( commandLine.get_num_arguments() < 2)
			{
				help->set_queues(true);
				help->set_datas(true);
				help->set_operations(true);
				
				
				delilah->network->send(delilah, delilah->network->controllerGetIdentifier(), Message::Help, p);
				return;
			}
			
			std::string secondCommand = commandLine.get_argument(1);
			
			if( secondCommand == "queues" )
			{
				// Ask for queues
				help->set_queues(true);
				help->set_datas(false);
				help->set_operations(false);
				delilah->network->send(delilah, delilah->network->controllerGetIdentifier(), Message::Help, p);
				return;
			}
			else if( secondCommand == "datas" )
			{
				// Ask for datas
				help->set_queues(false);
				help->set_datas(true);
				help->set_operations(false);

				delilah->network->send(delilah, delilah->network->controllerGetIdentifier(), Message::Help, p);

				return;
			}
			else if( secondCommand == "operations" )
			{
				// Ask for operations
				help->set_queues(false);
				help->set_datas(false);
				help->set_operations(true);

				delilah->network->send(delilah, delilah->network->controllerGetIdentifier(), Message::Help, p);

				return;
			}
			
			writeErrorOnConsole("Please, help [queues] [data_queues] [datas] [operations]");
			return;
			
			
		}
		
		if( mainCommand == "upload" )
		{
			if( commandLine.get_num_arguments() < 3)
			{
				writeErrorOnConsole("Usage: upload file <file2> .... <fileN> queue");
				return;
			}
			
			std::vector<std::string> fileNames;
			for (int i = 1 ; i < (commandLine.get_num_arguments()-1) ; i++)
			{
				std::string fileName = commandLine.get_argument(i);
				
				struct stat buf;
				stat( fileName.c_str() , &buf );
				
				if( S_ISREG(buf.st_mode) )
				{
					std::ostringstream message;
					message << "Including regular file " << fileName;
					writeOnConsole( message.str() );
					fileNames.push_back( fileName );
				}
				else if ( S_ISDIR(buf.st_mode) )
				{
					std::ostringstream message;
					message << "Including directory " << fileName;
					writeOnConsole( message.str() );
					
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
					std::ostringstream message;
					message << "Skipping " << fileName;
					writeOnConsole( message.str() );
				}
			}
			
			std::string queue = commandLine.get_argument( commandLine.get_num_arguments()-1 );
			
			size_t id = delilah->addUploadData(fileNames, queue);
			
			std::ostringstream o;
			o << "Load data process (id="<<id<<") started with " << fileNames.size() << " files\n";
			writeWarningOnConsole(o.str());
			return;
		}
		
		// Normal command send to the controller
		size_t id = delilah->sendCommand(command);
		
		id = 0;	// To avoid warning
		//std::ostringstream o;
		//o << "Sent command to controller (id="<<id<<") : " << command;
		//writeWarningOnConsole(o.str());
		
	}
	
	int DelilahConsole::receive(int fromId, Message::MessageCode msgCode, Packet* packet)
	{
		std::ostringstream  txt;
		bool                error = false;

		switch (msgCode) {
				
			case Message::CommandResponse:
			{
				
				if( packet->message.command_response().has_new_job_id() )
				{
					std::ostringstream message;
					message << "Job scheduled [" << packet->message.command_response().new_job_id() << "] ";
					message << " (" << packet->message.command_response().command() << " )";
					writeWarningOnConsole( message.str() );
				}
				
				if( packet->message.command_response().has_finish_job_id() )
				{
					std::ostringstream message;
					message << "Job finished [" << packet->message.command_response().finish_job_id() << "] ";
					message << " [ "<< au::Format::time_string( packet->message.command_response().ellapsed_seconds() ) << " ] ";
					message << " (" << packet->message.command_response().command() << " )";
					writeWarningOnConsole( message.str() );
				}
	
				if( packet->message.command_response().has_error_job_id() )
				{
					std::ostringstream message;
					message << "Job finished with error [" << packet->message.command_response().error_job_id() << "] ";
					message << " (" << packet->message.command_response().command() << ")\n\n";
					writeErrorOnConsole( message.str() );
				}
				
				if( packet->message.command_response().has_error_message() )
					writeErrorOnConsole( packet->message.command_response().error_message()  );

				if( packet->message.command_response().has_queue_list() )
					showQueues( packet->message.command_response().queue_list() );
				
				if( packet->message.command_response().has_automatic_operation_list() )
					showAutomaticOperations( packet->message.command_response().automatic_operation_list() );
				
				if( packet->message.command_response().has_data_list() )
					showDatas( packet->message.command_response().data_list() );
				
				if( packet->message.command_response().has_operation_list() )
					showOperations( packet->message.command_response().operation_list() );
				
				if( packet->message.command_response().has_job_list() )
					showJobs( packet->message.command_response().job_list() );
				
				if( packet->message.command_response().has_worker_status_list() )
					showWorkers( packet->message.command_response().controller_status(), packet->message.command_response().worker_status_list() );
			}
				break;
				
			case Message::HelpResponse:
			{
				network::HelpResponse help_response = packet->message.help_response();

				if( help_response.help().queues() )
				{
					txt << "Queues" << std::endl;
					txt << "------------------------------------------------------------------------------------------------" << std::endl;
					for (int i = 0 ; i < help_response.queue_size() ; i++)
					{
						network::Queue queue = help_response.queue(i).queue();

						txt << std::setw(30) << queue.name();
						txt << " ";
						txt << au::Format::string( queue.info().kvs() );
						txt << " kvs in ";
						txt << au::Format::string( queue.info().size() ) << " bytes";
						txt << " #File: " << help_response.queue(i).file_size();
						txt << " (" << queue.format().keyformat() << " " << queue.format().valueformat() << ") ";
						txt << std::endl;
					}
					txt << "------------------------------------------------------------------------------------------------" << std::endl;
					
					txt << std::endl;
				}
				
				if( help_response.help().datas() )
				{
					txt << "Datas" << std::endl;
					txt << "------------------------------------------------------------------------------------------------" << std::endl;
					for (int i = 0 ; i < help_response.data_size() ; i++)
					{
						network::Data data = help_response.data(i);
						txt << std::setw(20) << data.name() << " " << data.help() << std::endl;
					}
					txt << "------------------------------------------------------------------------------------------------" << std::endl;
				}

				if( help_response.help().operations() )
				{
					txt << "Operations" << std::endl;
					txt << "------------------------------------------------------------------------------------------------" << std::endl;
					for (int i = 0 ; i < help_response.operation_size() ; i++)
					{
						network::Operation operation = help_response.operation(i);
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
				}
				
			}
				break;
			default:
				txt << "Unknwn packet received\n";
				assert(false);
				break;
		}
		
		
		
		if (error)
			writeErrorOnConsole(txt.str());
		else
			writeOnConsole(txt.str());

		
		return 0;
	}	
	
	void DelilahConsole::loadDataConfirmation( DelilahUploadDataProcess *process)
	{
		std::ostringstream o;
		o << "Alert: Load data process finished\n";
		o << process->getStatus();
		writeWarningOnConsole(o.str());
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
		
		writeBlockOnConsole( txt.str() );		
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
		
		writeBlockOnConsole( txt.str() );
		
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
		
		writeBlockOnConsole( txt.str() );
		
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
		
		writeBlockOnConsole( txt.str() );
		
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
			
			txt << std::endl;
		}
		txt << "------------------------------------------------------------------------------------------------" << std::endl;
		txt << std::endl;
		
		writeBlockOnConsole( txt.str() );
		
	}
	
	void DelilahConsole::showWorkers( const network::ControllerStatus &cs , const network::WorkerStatusList l)
	{
		std::ostringstream txt;
		txt << "------------------------------------------------------------------------------------------------" << std::endl;
		txt << "Controller" << std::endl;
		txt << "------------------------------------------------------------------------------------------------" << std::endl;
		txt << "\tJobManager: " << cs.job_manager_status() << std::endl;
		txt << "\tTaskManager: " << cs.task_manager_status() << std::endl;
		txt << "------------------------------------------------------------------------------------------------" << std::endl;
		txt << "Workers" << std::endl;
		txt << "------------------------------------------------------------------------------------------------" << std::endl;
		for (int i = 0 ; i < l.worker_status_size() ; i++)
		{
			
			const network::WorkerStatus worker_status = l.worker_status(i);
			
			txt << "Worker " << i << "     " << "( Time of update: " << worker_status.time() << " )" << std::endl;
			txt << "\tMemory Manager: " << worker_status.memory_status() << "\n";
			txt << "\tFile Manager: " << worker_status.file_manager_status() << "\n";
			txt << "\tFile Manager Cache: " << worker_status.file_manager_cache_status() << "\n";
			txt << "\tData Buffer: " << worker_status.data_buffer_status() << "\n";
			txt << "\tLoad Data Manager: " << worker_status.load_data_manager_status() << "\n";
			txt << "\tDisk Manager: " << worker_status.disk_manager_status() << "\n";
			txt << "\tTask Manager: " << worker_status.task_manager_status() << "\n";

			txt << "\tProcess: ";
			for (int p = 0 ; p < worker_status.process_size() ; p++)
			{
				txt << "[";
				if( ! worker_status.process(p).working() )
					txt << "W";
				else
					txt << "T:" << worker_status.process(p).task_id() << ":" <<  worker_status.process(p).operation();
				txt <<  "]";
			}
			txt << "\n";
			
		}
		txt << "------------------------------------------------------------------------------------------------" << std::endl;
		txt << std::endl;

		writeBlockOnConsole( txt.str() );
		
	}
	
}
