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
#include "DelilahLoadDataProcess.h"		// ss::DelilahLoadDataProcess
#include "MemoryManager.h"				// ss::MemoryManager

namespace ss
{
	
	void cancel_ncurses(void)	
	{
		au::Console::cancel_ncurses();
	}
	
	
	void DelilahConsole::evalCommand( std::string command )
	{
		au::CommandLine commandLine;
		commandLine.set_flag_string("name", "no_name");
		commandLine.parse( command );

		std::string mainCommand;

		if( commandLine.get_num_arguments() == 0)
		{
			writeWarningOnConsole("Status by default");
			mainCommand = "status";	// Default command
		}
		else
			mainCommand = commandLine.get_argument(0);
		
		
		if ( mainCommand == "internal_status" )
		{
			
			std::ostringstream output;
			
			output << "Memory status: " << MemoryManager::shared()->getStatus();
			
			writeBlockOnConsole(output.str());
			
			output << "Load processes....\n";

			std::map<size_t,DelilahLoadDataProcess*>::iterator iter;
			for (iter = dalilah->loadProcess.begin() ; iter != dalilah->loadProcess.end() ; iter++)
				output << iter->second->getStatus();
			
			
			
			return ;
			
		}
		
		
		if( mainCommand == "quit" )
		{
			dalilah->quit();
			return;
		}

		if( mainCommand == "status" )
		{
			// Sent a status request to all the elements
			{
				Packet* p = new Packet();
				p->message.mutable_status_request()->set_command( command );
				dalilah->network->send(dalilah, dalilah->network->controllerGetIdentifier(), Message::StatusRequest, p);
			}

			for (int i = 0; i < dalilah->network->getNumWorkers(); i++)
			{
				Packet* p;
				int     workerId;

				p = new Packet();
				p->message.mutable_status_request()->set_command(command);

				workerId = dalilah->network->workerGetIdentifier(i);
				LM_M(("Sending Status to worker %d (endpoint id: %d)", i, workerId));
				dalilah->network->send(dalilah, workerId, Message::StatusRequest, p);
			}

			writeWarningOnConsole("Status messages sent to all elements\n");
			
			return;
		}
		
		if ( mainCommand == "help" )
		{
			
			// Ask for all help
			Packet*         p    = new Packet();
			network::Help*  help = p->message.mutable_help();

			if( commandLine.get_flag_string("name") != "no_name")
				help->set_name( commandLine.get_flag_string("name") );
			
			
			if( commandLine.get_num_arguments() < 2)
			{
				help->set_queues(true);
				help->set_datas(true);
				help->set_operations(true);
				
				
				dalilah->network->send(dalilah, dalilah->network->controllerGetIdentifier(), Message::Help, p);
				return;
			}
			
			std::string secondCommand = commandLine.get_argument(1);
			
			if( secondCommand == "queues" )
			{
				// Ask for queues
				help->set_queues(true);
				help->set_datas(false);
				help->set_operations(false);
				dalilah->network->send(dalilah, dalilah->network->controllerGetIdentifier(), Message::Help, p);
				return;
			}
			else if( secondCommand == "datas" )
			{
				// Ask for datas
				help->set_queues(false);
				help->set_datas(true);
				help->set_operations(false);

				dalilah->network->send(dalilah, dalilah->network->controllerGetIdentifier(), Message::Help, p);

				return;
			}
			else if( secondCommand == "operations" )
			{
				// Ask for operations
				help->set_queues(false);
				help->set_datas(false);
				help->set_operations(true);

				dalilah->network->send(dalilah, dalilah->network->controllerGetIdentifier(), Message::Help, p);

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
			
			size_t id = dalilah->loadData(fileNames, queue);
			
			std::ostringstream o;
			o << "Load data process (id="<<id<<") started with " << fileNames.size() << " files\n";
			writeWarningOnConsole(o.str());
			return;
		}
		
		// Normal command send to the controller
		Packet*           p = new Packet();
		network::Command* c = p->message.mutable_command();

		c->set_command( command );
		c->set_sender_id( id++ );
		dalilah->network->send(dalilah, dalilah->network->controllerGetIdentifier(), Message::Command, p);

		
		std::ostringstream o;
		o << "Sent command to controller (id="<<id-1<<") : " << command;
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
				txt << "Answer for command " << id << ": " << command << std::endl;
				
				if (finished)
				{
					if (error)
						txt << "Command finished with error" << std::endl;
					else
						txt << "Command finished correctly" << std::endl;
				}
				
				txt << std::endl;
				txt << "----------------------------------------------------------------" << std::endl;
				txt << message << std::endl;
				txt << "----------------------------------------------------------------" << std::endl;
			}
				break;
				
			case Message::HelpResponse:
			{
				network::HelpResponse help_response = packet->message.help_response();

				if( help_response.help().queues() )
				{
					txt << "Queues" << std::endl;
					txt << "------------------------------------------------" << std::endl;
					for (int i = 0 ; i < help_response.queue_size() ; i++)
					{
						network::Queue queue = help_response.queue(i);
						txt << queue.name();
						txt << " (" << queue.format().keyformat() << "-" << queue.format().valueformat() << ") ";
						txt << " " << au::Format::string( queue.info().kvs() ) << " kvs in " << au::Format::string( queue.info().size() ) << " bytes" << std::endl;
					}
					txt << "------------------------------------------------" << std::endl;
					
					txt << std::endl;
				}
				
				if( help_response.help().datas() )
				{
					txt << "Datas" << std::endl;
					txt << "------------------------------------------------" << std::endl;
					for (int i = 0 ; i < help_response.data_size() ; i++)
					{
						network::Data data = help_response.data(i);
						txt << data.name() << " - " << data.help() << std::endl;
					}
					txt << "------------------------------------------------" << std::endl;
				}

				if( help_response.help().operations() )
				{
					txt << "Operations" << std::endl;
					txt << "------------------------------------------------" << std::endl;
					for (int i = 0 ; i < help_response.operation_size() ; i++)
					{
						network::Operation operation = help_response.operation(i);
						txt << operation.name() << " [ " << operation.input_size()  << " -> " << operation.output_size() << " ]" <<  " - " << operation.help() << std::endl;
					}
					txt << "------------------------------------------------" << std::endl;
				}
				
			}
				break;
			default:
				txt << "Unknwn packet received\n";
				break;
		}
		
		
		
		if (error)
			writeErrorOnConsole(txt.str());
		else
			writeOnConsole(txt.str());

		
		return 0;
	}	
	
	void DelilahConsole::loadDataConfirmation( DelilahLoadDataProcess *process)
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
