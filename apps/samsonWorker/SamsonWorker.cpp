#include <iostream>				// std::cout ...

#include "logMsg.h"             // lmInit, LM_*
#include "traceLevels.h"        // LMT_*

#include "Macros.h"             // EXIT, ...
#include "Packet.h"				// ss::Packet
#include "network.h"			// NetworkInterface
#include "Endpoint.h"			// Endpoint
#include "CommandLine.h"		// CommandLine
#include "SamsonWorker.h"		// Own interfce



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
 Main routine for the samsonWorker
 */

int main(int argc, const char *argv[])
{
	LmStatus  s;

	progName = lmProgName((char*) argv[0], 1, true);

	if ((s = lmPathRegister("/tmp/", "DEF", "DEF", NULL)) != LmsOk)
		EXIT(1, ("lmPathRegister: %s", lmStrerror(s)));
	if ((s = lmFdRegister(1, "TYPE: TEXT", "DEF", "controller", NULL)) != LmsOk)
		EXIT(1, ("lmPathRegister: %s", lmStrerror(s)));
	if  ((s = lmInit()) != LmsOk)
		EXIT(1, ("lmInit: %s", lmStrerror(s)));

	ss::SamsonWorker worker(argc, argv);
	worker.run();
}
