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
#include "Job.h"						// Environment operations (CopyFrom)
#include <iomanip>

namespace ss
{
	
	void cancel_ncurses(void)	
	{
		au::Console::cancel_ncurses();
	}
	
	
	void DelilahConsole::evalCommand( std::string command )
	{
		au::CommandLine commandLine;
		commandLine.set_flag_string("name", "null");
		commandLine.set_flag_string("begin", "null");
		commandLine.set_flag_boolean("show");
		commandLine.parse( command );

		std::string mainCommand;

		if( commandLine.get_num_arguments() == 0)
		{
			writeWarningOnConsole("Status by default");
			mainCommand = "status";	// Default command
		}
		else
			mainCommand = commandLine.get_argument(0);
		
		
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
		
		
		
		if ( mainCommand == "internal_status" )
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

		if( mainCommand == "status" )
		{
			// Send a status request to all elements
			{
				Packet* p = new Packet();
				p->message.mutable_status_request()->set_command( command );
				delilah->network->send(delilah, delilah->network->controllerGetIdentifier(), Message::StatusRequest, p);
			}

			/*

			 // Status requests go through controller
			 
			for (int i = 0; i < delilah->network->getNumWorkers(); i++)
			{
				Packet* p = new Packet();
				int     workerId;

				p->message.mutable_status_request()->set_delilah_id(19);

				workerId = delilah->network->workerGetIdentifier(i);
				LM_M(("Sending Status to worker %d (endpoint id: %d)", i, workerId));
				delilah->network->send(delilah, workerId, Message::StatusRequest, p);
			}
			 */

			writeWarningOnConsole("Status messages sent to all elements\n");
			
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
		
		if( mainCommand == "load" )
		{
			if( commandLine.get_num_arguments() < 3)
			{
				writeErrorOnConsole("Usage: load file <file2> .... <fileN> queue");
				return;
			}
			
			std::vector<std::string> fileNames;
			for (int i = 1 ; i < (commandLine.get_num_arguments()-1) ; i++)
				fileNames.push_back( commandLine.get_argument(i) );
			
			std::string queue = commandLine.get_argument( commandLine.get_num_arguments()-1 );
			
			size_t id = delilah->addUploadData(fileNames, queue);
			
			std::ostringstream o;
			o << "Load data process (id="<<id<<") started with " << fileNames.size() << " files\n";
			writeWarningOnConsole(o.str());
			return;
		}
		
		// Normal command send to the controller
		size_t id = delilah->sendCommand(command);
		
		std::ostringstream o;
		o << "Sent command to controller (id="<<id<<") : " << command;
		writeWarningOnConsole(o.str());
		
	}
	
	int DelilahConsole::receive(int fromId, Message::MessageCode msgCode, Packet* packet)
	{
		std::ostringstream  txt;
		bool                error = false;
		

		switch (msgCode) {

			case Message::StatusResponse:
			{
				// Get some information form the packet
				std::string title = packet->message.status_response().title();
				std::string message = packet->message.status_response().response();

				// Prepare what to show on screen
				txt << "----------------------------------------------------------------" << std::endl;
				txt << "STATUS " << title << std::endl;
				txt << "----------------------------------------------------------------" << std::endl;
				txt << message << std::endl;
				txt << "----------------------------------------------------------------" << std::endl;
			}
				break;				
				
			case Message::CommandResponse:
			{
				// Get some information from the packet
				size_t      id        = packet->message.command_response().sender_id();
				std::string command   = packet->message.command_response().command();
				std::string message   = packet->message.command_response().response();
				error                 = packet->message.command_response().error();
				bool        finished  = packet->message.command_response().finish();
				
				// Prepare what to show on screen
				txt << "----------------------------------------------------------------" << std::endl;
				txt << "COMMAND: " << id << ": " << command << std::endl;
				txt << "----------------------------------------------------------------" << std::endl;
				
				if (finished)
				{
					if (error)
						txt << "Command finished with error" << std::endl;
					else
						txt << "Command finished correctly" << std::endl;
				}
				txt << "----------------------------------------------------------------" << std::endl;
				txt << std::endl;
				txt << message << std::endl;
				txt << std::endl;
				txt << "----------------------------------------------------------------" << std::endl;
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
		o << "Alert: Load data process (id=" << process->getId() << ") finished\n";
		o << "Total uploaded: " << au::Format::string( process->getUploadedSize() ) << "bytes\n";
		
		std::vector<std::string> failedFiles = process->getFailedFiles();
		if( failedFiles.size() == 0 )
			o << "No failed files\n";
		else
		{
			o << failedFiles.size() << " Failed files\n";
			for (size_t i = 0 ; i < failedFiles.size() ; i++)
				o << "\tFile: " << failedFiles[i] << std::endl;
		}
		
		
		std::vector<network::File> created_file  = process->getCreatedFile();
		o<< created_file.size() << " created files:\n";
		for (size_t i = 0 ; i < created_file.size() ; i++)
		{
			o << "\tFile " << created_file[i].name();
			o << " W:" << created_file[i].worker();
			o << " Size: " << au::Format::string( created_file[i].info().size() );
			//o << " KVS: " << au::Format::string( created_file[i].info().kvs() );
			o << std::endl;
		}
			
		writeWarningOnConsole(o.str());
	};
	
	
}
