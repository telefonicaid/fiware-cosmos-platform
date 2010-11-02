#include <iostream>				// std::cout ...

#include "logMsg.h"             // lmInit, LM_*

#include "Macros.h"             // EXIT, ...
#include "Message.h"            // Message::MessageCode, ...
#include "Packet.h"				// ss::Packet
#include "Network.h"			// NetworkInterface
#include "Endpoint.h"			// Endpoint
#include "CommandLine.h"		// CommandLine
#include "Delilah.h"			// Own interfce
#include "Packet.h"				// ss::Packet

namespace ss {


	void* runNetworkThread(void *p)
	{
		Delilah *d = ((Delilah*)p);
		d->runNetwork();
		return NULL;
	}
	
	void Delilah::runNetwork()
	{
		// Main run_loop to the network interface
		network->run();	
	}

	
	void Delilah::run()
	{
		// Run a console or a graphical interface ( independently if network interface is created or not )
		assert( console || interfaceQt );	// One of the interface has to be active
		if ( console )
			console->run();
		else if( interfaceQt )
			interfaceQt->run(_argc ,_argv );
	}
	
	
	size_t Delilah::sendMessageToController( std::string message )
	{
		Packet p;

		network::Command * command = p.message.mutable_command();
		command->set_sender_id( local_id++ );
		command->set_command( message );
		
		network->send(this, network->controllerGetIdentifier(), Message::Command, NULL, 0, &p);
		
		return (local_id - 1);
	}
	
	int Delilah::receivedMessage( size_t id , bool error , bool finished , std::string message )
	{
		assert( console || interfaceQt );
		
		if (console)
			console->receivedMessage( id , error , finished , message );
		else
			interfaceQt->receivedMessage( id , error , finished , message );

		
		
		return 0;
	}
	

	int Delilah::receive(int fromId, Message::MessageCode msgCode, void* dataP, int dataLen, Packet* packet)
	{
		if (msgCode == Message::CommandResponse)
		{
			std::string message  = packet->message.command_response().response();
			bool        error    = packet->message.command_response().error();
			bool        finish   = packet->message.command_response().finish();
			size_t      id       = packet->message.command_response().sender_id();
			
			return receivedMessage(id, error, finish, message);
		}

		LM_RE(1, ("Unknown message code %d", msgCode));
	}
	
	void Delilah::notificationSent(size_t id, bool success)
	{
		// Do something
	}

}

