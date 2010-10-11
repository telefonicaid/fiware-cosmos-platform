

#include "Delilah.h"			// Own interfce
#include "packet.h"				// ss::Packet
#include <iostream>				// std::cout ...
#include "network.h"			// NetworkInterface
#include "packet.h"				// Packet
#include "endpoint.h"			// EndPoint
#include "CommandLine.h"		// CommandLine


namespace ss {

	void* runTest(void *p)
	{
		((Delilah*)p)->test();
		return NULL;
	}
	
	
	void Delilah::run()
	{
		// Run a test in a paralel thread
		pthread_t t;
		pthread_create(&t, NULL, runTest, this);

		// Main run_loop to the network interface
		network.run();	
	}

	void Delilah::test()
	{
		while( ! network.ready() )
		{
			std::cout << "Waiting for the network interface to be ready" << std::endl;
			sleep(1);
		}
		
		// Testing to sent something to the controller
		while( true )
		{
			ss::Packet p;
			p.message.set_command("Hello there");	// Init the command inside the message
			p.buffer.initPacketBuffer(100);			// Init with the buffer with 100 garbage bytes
			
			network.send( &p , network.controller() , NULL );
			
			sleep(2);
		}		
	}
	

	void Delilah::receive( Packet *p , EndPoint fromEndPoint )
	{
		// Do something with it
		std::cout << "Delailah received a packet from " << fromEndPoint.str() << " -> " << p->str();
	}
	
	void Delilah::notificationSent( size_t id , bool success )
	{
		// Do something
	}

}

int main(int arg , const char *argv[])
{
	// Run delailah
	ss::Delilah delailah( arg , argv );
	delailah.run();
	
}
