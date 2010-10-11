#include <iostream>				// std::cout ...

#include "Delilah.h"			// Own interfce
#include "Packet.h"				// ss::Packet
#include "network.h"			// NetworkInterface
#include "Endpoint.h"			// Endpoint
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
			Packet p;
			p.message.set_command("Hello there");	// Init the command inside the message
			p.buffer.initPacketBuffer(100);			// Init with the buffer with 100 garbage bytes
			
			network.send( &p, network.controllerGet(), NULL );
			
			sleep(2);
		}		
	}
	

	void Delilah::receive( Packet *p , Endpoint* fromEndpoint )
	{
		// Do something with it
		std::cout << "Delilah received a packet from " << fromEndpoint->str() << " -> " << p->str();
	}
	
	void Delilah::notificationSent( size_t id , bool success )
	{
		// Do something
	}

}

int main(int arg , const char *argv[])
{
	ss::Delilah delilah( arg , argv );
	delilah.run();
}
