#include <iostream>				// std::cout ...

#include "logMsg.h"             // lmInit, LM_*
#include "traceLevels.h"        // LMT_*

#include "Macros.h"             // EXIT, ...
#include "Packet.h"				// ss::Packet
#include "network.h"			// NetworkInterface
#include "Endpoint.h"			// EndPoint
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
			
			std::vector<Endpoint> workers = network.samsonWorkerEndpoints();
			
			Packet p;
			p.message.set_command("Hello there from controller");	// Init the command inside the message
			p.buffer.initPacketBuffer(200);							// Init with the buffer with 100 garbage bytes
			
			for (std::vector<Endpoint>::iterator e = workers.begin() ; e != workers.end() ; e++)
				network.send(&p, &*e, NULL);
			
			sleep(1);
		}
		
	}
	
	

	
	
	void SamsonController::receive( Packet *p , Endpoint* fromEndPoint )
	{
		// Do something with it
		std::cout << "Packet received from " << fromEndPoint->str() << " -> " << p->str();
	}

	void SamsonController::notificationSent( size_t id , bool success )
	{
		// Do something
	}
	
	
}

/**
 Main routine for the samsonController
 */

int main(int argc, const char* argv[])
{
	LmStatus  s;
	char*     trace = (char*) "0-255";

	progName = lmProgName((char*) argv[0], 1, false);

	if ((s = lmPathRegister("/tmp/", "DEF", "DEF", NULL)) != LmsOk)
		EXIT(1, ("lmPathRegister: %s", lmStrerror(s)));
	if ((s = lmFdRegister(1, "TYPE: TEXT", "DEF", "controller", NULL)) != LmsOk)
		EXIT(1, ("lmPathRegister: %s", lmStrerror(s)));
	if  ((s = lmInit()) != LmsOk)
		EXIT(1, ("lmInit: %s", lmStrerror(s)));

	if ((argc >= 3) && (strcmp(argv[1], "-t") == 0))
		trace = (char*) argv[2];
	if ((s = lmTraceSet(trace)) != LmsOk)
		EXIT(1, ("lmTraceSet: %s", lmStrerror(s)));

	LM_F(("set trace levels to '%s'", trace));
	for (int ix = 0; ix < 256; ix++)
		LM_T(ix,  ("Testing trace level %d", ix));

	ss::SamsonController controller(argc, argv);
	controller.run();
}
