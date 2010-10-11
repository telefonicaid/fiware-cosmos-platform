

#include "SamsonWorker.h"		// Own interfce
#include <iostream>				// std::cout ...
#include "Packet.h"				// ss::Packet
#include "network.h"			// NetworkInterface
#include "Endpoint.h"			// Endpoint
#include "CommandLine.h"		// CommandLine
#include "SamsonWorker.h"		// ss::SamsonWorker



namespace ss {


	void* runTest(void *p)
	{
		((SamsonWorker*)p)->test();
		return NULL;
	}
	
	void SamsonWorker::run()
	{
		pthread_t p;
		pthread_create(&p, NULL, runTest, this);
		network.run();
	}
	
	
	void SamsonWorker::test()
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
			
			network.send(&p, network.controllerGet(), NULL);
			
			sleep(2);
		}
		
	}


	void SamsonWorker::receive( Packet* p, Endpoint* fromEndPoint )
	{
		// Do something with it
		std::cout << "Packet received from " << fromEndPoint->str() << " -> " << p->str();
	}
	
	void SamsonWorker::notificationSent( size_t id , bool success )
	{
		// Do something
	}
	
	
}

/**
 Main routire for the samsonWorker
 */

int main(int arg , const char *argv[])
{

	// Run the worker
	ss::SamsonWorker worker( arg , argv );
	worker.run();

	
}
