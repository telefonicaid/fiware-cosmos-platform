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
			LM_T(LMT_READY, ("Awaiting network interface ready"));
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



/* ****************************************************************************
*
* main - main routine for the samsonController
*/
#define LOG_FORMAT "TYPE:EXEC:FILE[LINE]:FUNC: TEXT"
int main(int argc, const char* argv[])
{
	LmStatus  s;

	progName = lmProgName((char*) argv[0], 1, false);

	if ((s = lmPathRegister("/tmp/", LOG_FORMAT, "DEF", NULL)) != LmsOk)
		EXIT(1, ("lmPathRegister: %s", lmStrerror(s)));
	if ((s = lmFdRegister(1, "TYPE: TEXT", "DEF", "controller", NULL)) != LmsOk)
		EXIT(1, ("lmPathRegister: %s", lmStrerror(s)));
	if  ((s = lmInit()) != LmsOk)
		EXIT(1, ("lmInit: %s", lmStrerror(s)));

	ss::SamsonController controller(argc, argv);
	controller.run();
}
