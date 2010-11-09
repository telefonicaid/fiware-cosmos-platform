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
	

	int Delilah::receive(int fromId, Message::MessageCode msgCode, Packet* packet)
	{
		
		// Forward the reception of this message to the client
		assert( console || interfaceQt );
		
		if (console)
			console->receive( fromId , msgCode , packet );
		else
			interfaceQt->receive( fromId , msgCode ,packet );
		
		return 0;
	}
	
	void Delilah::notificationSent(size_t id, bool success)
	{
		// Do something
	}

}

