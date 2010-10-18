#include <iostream>				// std::cout ...

#include "logMsg.h"             // lmInit, LM_*

#include "Macros.h"             // EXIT, ...
#include "Packet.h"				// ss::Packet
#include "Network.h"			// NetworkInterface
#include "Endpoint.h"			// EndPoint
#include "CommandLine.h"		// CommandLine
#include "SamsonController.h"	// own interface ss::SamsonController
#include "ModulesManager.h"		// ss:ModulesManager
#include "ControllerTaskManager.h"		// ss:ControllerTask

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
			
			std::vector<Endpoint*> workers = network->samsonWorkerEndpoints();
			
			Packet p;
			p.message.set_command("Hello there from controller");	// Init the command inside the message
			p.buffer.initPacketBuffer(200);							// Init with the buffer with 100 garbage bytes
			
			for (std::vector<Endpoint*>::iterator e = workers.begin() ; e != workers.end() ; e++)
				network->send(&p, *e, NULL);
			
			sleep(1);
		}
		
	}
	
	
	void SamsonController::sendDalilahAnswer( size_t sender_id ,  Endpoint *dalilahEndPoint , bool error , std::string answer_message  )
	{
		// Get status of controller
		Packet p2;
		p2.message.set_code( 0 );
		p2.message.set_answer( answer_message );
		p2.message.set_error( error );
		p2.message.set_sender_id( sender_id );
		network->send(&p2, dalilahEndPoint, this);
	}
	
	void SamsonController::sendWorkerTasks( ControllerTask *task )
	{
		 // Send messages to the workers indicating the operation to do ( waiting the confirmation from all of them )
		 for (size_t i = 0 ; i < workerEndPoints.size() ; i++)
			 sendWorkerTask( &workerEndPoints[i] , task->getId() , task->getCurrentCommand() );
	}	
	
	void SamsonController::sendWorkerTask( Endpoint * worker , size_t task_id , std::string command )
	{
		// Get status of controller
		Packet p2;
		p2.message.set_code( 0 );

		network::WorkerTask *t = p2.message.mutable_task();
		t->set_command( command );
		t->set_task_id( task_id );
		
		network->send(&p2, worker, this);
	}
	
	
	void SamsonController::receive( Packet *p , Endpoint* fromEndPoint )
	{
		
		if( p->messageCodeGet() == Packet::DalilahCommand )
		{
			
			au::CommandLine cmdLine;
			cmdLine.parse( p->message.command() );
			std::ostringstream output;					// General output string buffer
			
			if( cmdLine.get_num_arguments() == 0)
				return;
			
			// General status command
			if( cmdLine.get_argument(0) == "status" )
			{
				output << "Status of controller" << std::endl;			
				output << "====================" << std::endl;
				output << data.status();
				
				
				// Get status of controller
				sendDalilahAnswer( p->message.sender_id() , fromEndPoint , false , output.str() );
				return;
			}
			
			// Try to schedule the command
			bool success = taskManager.addTask( p->message.command() , output  );
			
			// Send something back to dalilah
			sendDalilahAnswer( p->message.sender_id() , fromEndPoint , !success , output.str() );
			
			return;
		}
		
		if( p->messageCodeGet() == Packet::WorkerTaskConfirmation )
		{
			// A confirmation from a worker is received
			
			size_t task_id = p->message.task_confirmation().task_id();
			int worker_id = network->worker(fromEndPoint);
			
			taskManager.notifyWorkerConfirmation( task_id , worker_id );			
		}
		
	}

	void SamsonController::notificationSent( size_t id , bool success )
	{
		// Do something
	}
	
	
}



