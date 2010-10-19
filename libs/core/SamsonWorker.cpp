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
		//ss::Packet p;
		//p.message.set_command("Hello there");	// Init the command inside the message
		//p.buffer.initPacketBuffer(100);			// Init with the buffer with 100 garbage bytes
		//network->send(&p, network->controllerGet(), NULL);
		sleep(2);
	}
}


void SamsonWorker::receive(Packet* p, int from)
{
	
	if( p->message.type() == network::Message_Type_WorkerTask )
	{
		// Sent a confirmation just to test everything is ok
		Packet p2( network::Message_Type_WorkerTaskConfirmation );
		
		network::WorkerTaskConfirmation * confirmation = p2.message.mutable_worker_task_confirmation();
		confirmation->set_task_id( p->message.worker_task().task_id() );
		confirmation->set_error( false );
		
		network->send(&p2, network->controllerGetIdentifier(), this);
	}
	
}
	

void SamsonWorker::notificationSent( size_t id , bool success )
{
	// Do something
}

}
