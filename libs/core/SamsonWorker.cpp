#include <iostream>				// std::cout ...

#include "logMsg.h"             // lmInit, LM_*

#include "Macros.h"             // EXIT, ...
#include "Packet.h"				// ss::Packet
#include "Network.h"			// NetworkInterface
#include "Endpoint.h"			// Endpoint
#include "CommandLine.h"		// CommandLine
#include "SamsonWorker.h"		// Own interfce

#define LMT_READY 21	

namespace ss {


void* runSamsonControllerTest(void *p)
{
	((SamsonWorker*)p)->test();
	return NULL;
}
	
void SamsonWorker::run()
{
	assert( network );
	
	pthread_t p;
	pthread_create(&p, NULL, runSamsonControllerTest, this);
	network->run();
}
	
	
void SamsonWorker::test()
{
	while( ! network->ready() )
	{
		LM_T(LMT_READY, ("Awaiting network interface ready"));
		sleep(5);
	}
		
	LM_T(LMT_READY, ("*********** READY !!! ***********"));
	return;

	// Testing to sent something to the controller
	while( true )
	{
		ss::Packet p;
		p.message.set_command("Hello there");	// Init the command inside the message
		p.buffer.initPacketBuffer(100);			// Init with the buffer with 100 garbage bytes
			
		network->send(&p, network->controllerGet(), NULL);
			
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
