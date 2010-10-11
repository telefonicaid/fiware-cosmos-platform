

#include <iostream>				// std::cout ...
#include "packet.h"						// ss::Packet
#include "network.h"			// NetworkInterface
#include "packet.h"				// Packet
#include "endpoint.h"			// EndPoint
#include "CommandLine.h"		// CommandLine
#include "SamsonController.h"	// own interface ss::SamsonController


namespace ss {

	
	void* runTest(void *p)
	{
		((SamsonController*)p)->test();
		return NULL;
	}
	
	void SamsonController::run()
	{
		pthread_t p;
		pthread_create(&p, NULL, runTest, this);
		network.run();
	}
	
	void SamsonController::test()
	{
	
		// Testing receiving packets and sending to all the workers
		
		while( ! network.ready() )
		{
			std::cout << "Waiting for the network interface to be ready" << std::endl;
			sleep(1);
		}
		
		while( true )
		{
			
			std::vector<EndPoint> workers = network.samsonWorkersEndPoints();
			
			Packet p;
			p.message.set_command("Hello there from controller");	// Init the command inside the message
			p.buffer.initPacketBuffer(200);							// Init with the buffer with 100 garbage bytes
			
			for (std::vector<EndPoint>::iterator e = workers.begin() ; e != workers.end() ; e++)
				network.send(&p, *e, NULL);
			
			sleep(1);
		}
		
	}
	
	

	
	
	void SamsonController::receive( Packet *p , EndPoint fromEndPoint )
	{
		// Do something with it
		std::cout << "Packet received from " << fromEndPoint.str() << " -> " << p->str();
	}

	void SamsonController::notificationSent( size_t id , bool success )
	{
		// Do something
	}
	
	
}

/**
 Main routine for the samsonController
 */

int main(int arg , const char *argv[])
{

	ss::SamsonController controller( arg , argv );
	controller.run();
	
}
