#include <iostream>				// std::cout ...

#include "logMsg.h"             // lmInit, LM_*

#include "Macros.h"             // EXIT, ...
#include "Packet.h"				// ss::Packet
#include "Network.h"			// NetworkInterface
#include "Endpoint.h"			// EndPoint
#include "CommandLine.h"		// CommandLine
#include "SamsonController.h"	// own interface ss::SamsonController

#define LMT_READY 21	


namespace ss {

	
	void* runTestSamsonController(void *p)
	{
		((SamsonController*)p)->test();
		return NULL;
	}
	
	void SamsonController::run()
	{
		pthread_t p;
		pthread_create(&p, NULL, runTestSamsonController, this);
		network->run();
	}
	
	void SamsonController::test()
	{
	
		// Testing receiving packets and sending to all the workers
		
		while( ! network->ready() )
		{
			LM_T(LMT_READY, ("Awaiting network interface ready"));
			sleep(5);
		}

		LM_T(LMT_READY, ("*********** READY !!! ***********"));
		return;

		while( true )
		{
			
			std::vector<Endpoint> workers = network->samsonWorkerEndpoints();
			
			Packet p;
			p.message.set_command("Hello there from controller");	// Init the command inside the message
			p.buffer.initPacketBuffer(200);							// Init with the buffer with 100 garbage bytes
			
			for (std::vector<Endpoint>::iterator e = workers.begin() ; e != workers.end() ; e++)
				network->send(&p, &*e, NULL);
			
			sleep(1);
		}
		
	}
	
	void SamsonController::receive( Packet *p , Endpoint* fromEndPoint )
	{
		
		au::CommandLine cmdLine;
		cmdLine.parse( p->message.command() );

		if( cmdLine.get_num_arguments() == 0)
			return;

		if( cmdLine.get_argument(0) == "status" )
		{
			// Get status of controller
			Packet p2;
			p2.message.set_code( 0 );
			p2.message.set_answer("Status of controller");
			p2.message.set_sender_id( p->message.sender_id() );
			
			network->send(&p2, fromEndPoint, this);
		
		}
		
		
	}

	void SamsonController::notificationSent( size_t id , bool success )
	{
		// Do something
	}
	
	
}



