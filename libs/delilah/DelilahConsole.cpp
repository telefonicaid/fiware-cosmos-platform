/* ****************************************************************************
*
* FILE                     DelilahConsole.h
*
* DESCRIPTION			   Console terminal for delilah
*
*/
#include "Delilah.h"			// ss::Delailh
#include "DelilahConsole.h"		// Own interface
#include "Packet.h"				// ss:Packet
#include "Format.h"				// au::Format

namespace ss
{
	void DelilahConsole::evalCommand( std::string command )
	{
		au::CommandLine commandLine;
		commandLine.parse( command );
		
		if( commandLine.get_num_arguments() == 0)
			return;	// no command
		
		
		std::string mainCommand = commandLine.get_argument(0);
		
		if( mainCommand == "quit" )
			dalilah->quit();
		else
		{
			if ( mainCommand == "help" )
			{
				if( commandLine.get_num_arguments() < 2)
				{
					// Ask for all help
					Packet p;
					network::Help *help = p.message.mutable_help();
					help->set_queues(true);
					help->set_datas(true);
					help->set_operations(true);
					dalilah->network->send(dalilah, dalilah->network->controllerGetIdentifier(), Message::Help, &p);
					return;	
				}
				
				std::string secondCommand = commandLine.get_argument(1);
				
				if( secondCommand == "queues" )
				{
					// Ask for queues
					Packet p;
					network::Help *help = p.message.mutable_help();
					help->set_queues(true);
					help->set_datas(false);
					help->set_operations(false);
					dalilah->network->send(dalilah, dalilah->network->controllerGetIdentifier(), Message::Help, &p);
					return;
				}
				else if( secondCommand == "datas" )
				{
					// Ask for datas
					Packet p;
					network::Help *help = p.message.mutable_help();
					help->set_queues(false);
					help->set_datas(true);
					help->set_operations(false);
					dalilah->network->send(dalilah, dalilah->network->controllerGetIdentifier(), Message::Help, &p);
					return;
				}
				else if( secondCommand == "operations" )
				{
					// Ask for operations
					Packet p;
					network::Help *help = p.message.mutable_help();
					help->set_queues(false);
					help->set_datas(false);
					help->set_operations(true);
					dalilah->network->send(dalilah, dalilah->network->controllerGetIdentifier(), Message::Help, &p);
					return;
				}
				
				writeErrorOnConsole("Please, help [queues] [datas] [operations]");
				return;
				
				
			}
			
			// Normal command send to the controller
			Packet p;
			network::Command *c = p.message.mutable_command();
			c->set_command( command );
			c->set_sender_id( id++ );
			dalilah->network->send(dalilah, dalilah->network->controllerGetIdentifier(), Message::Command, &p);

			
			std::ostringstream o;
			o << "Sent command to controller (id="<<id-1<<") : " << command;
			writeWarningOnConsole(o.str());
			
			
		}
	}
	
	int DelilahConsole::receive(int fromId, Message::MessageCode msgCode, Packet* packet)
	{
		std::ostringstream txt;
		bool error = false;
		
		switch (msgCode) {
				
			case Message::CommandResponse:
			{
				// Get some information form the packet
				size_t id = packet->message.command_response().sender_id();
				std::string message = packet->message.command_response().response();
				error = packet->message.command_response().error();
				bool finished = packet->message.command_response().finish();
				
				// Prepare what to show on screen
				txt << "----------------------------------------------------------------" << std::endl;
				txt << "Answer for command " << id << ": ";
				
				if( finished )
					txt << "(FINISHED)";
				
				txt << std::endl;
				txt << "----------------------------------------------------------------" << std::endl;
				txt << message << std::endl;
				txt << "----------------------------------------------------------------" << std::endl;
			}
				break;
				
			case Message::HelpResponse:
			{
				network::HelpResponse help_response = packet->message.help_response();

				if( help_response.queues() )
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
				}
				
				if( help_response.datas() )
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

				if( help_response.operations() )
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
}
