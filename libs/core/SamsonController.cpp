#include <iostream>				        // std::cout ...

#include "logMsg.h"                     // lmInit, LM_*

#include "Macros.h"                     // EXIT, ...
#include "Packet.h"				        // ss::Packet
#include "Network.h"			        // NetworkInterface
#include "Endpoint.h"			        // EndPoint
#include "CommandLine.h"		        // CommandLine
#include "ModulesManager.h"		        // ss:ModulesManager
#include "ControllerTaskManager.h"		// ss:ControllerTaskManager
#include "ControllerTask.h"				// ss:ControllerTask
#include "SamsonController.h"	        // Own interface ss::SamsonController



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
		
		while (!network->ready())
		{
			LM_T(LMT_READY, ("Awaiting network interface ready"));
			sleep(5);
		}

		LM_T(LMT_READY, ("*********** READY !!! ***********"));
		return;

		while (true)
		{
#if 0
			std::vector<Endpoint*> workers = network->samsonWorkerEndpoints();
			Packet p;
			p.message.set_command("Hello there from controller");	// Init the command inside the message
			p.buffer.initPacketBuffer(200);							// Init with the buffer with 100 garbage bytes			
			for (std::vector<Endpoint*>::iterator e = workers.begin() ; e != workers.end() ; e++)
				network->send(&p, *e, NULL);
#endif
			sleep(1);
		}
		
	}
	
	
	void SamsonController::sendDalilahAnswer( size_t sender_id , int dalilahIdentifier , bool error , bool finished, std::string answer_message )
	{
		// Get status of controller
		Packet p2( network::Message_Type_CommandResponse );
		
		network::CommandResponse *response = p2.message.mutable_command_response();
		response->set_response( answer_message );
		response->set_error( error );
		response->set_finish( finished );
		response->set_sender_id( sender_id );
		
		network->send(&p2, dalilahIdentifier , this);
	}
	
	void SamsonController::sendWorkerTasks( ControllerTask *task )
	{
		 // Send messages to the workers indicating the operation to do ( waiting the confirmation from all of them )
		 for (int i = 0 ; i < network->getNumWorkers() ; i++)
			 sendWorkerTask( i , task->getId() , task->getCurrentCommand() );
	}	
	
	void SamsonController::sendWorkerTask( int workerIdentifier , size_t task_id , std::string command )
	{
		// Get status of controller
		Packet p2( network::Message_Type_WorkerTask );

		network::WorkerTask *t = p2.message.mutable_worker_task();
		t->set_command( command );
		t->set_task_id( task_id );
		
		network->send(&p2, workerIdentifier, this);
	}
	
	
	void SamsonController::receive( Packet *p , int fromIdentifier )
	{
		
		if( p->message.type() == network::Message_Type_Command )
		{
			
			au::CommandLine cmdLine;
			cmdLine.parse( p->message.command().command() );
			std::ostringstream output;					// General output string buffer
			
			if( cmdLine.get_num_arguments() == 0)
				return;
			
			// General status command
			if( cmdLine.get_argument(0) == "status" )
			{
				output << "Status of controller" << std::endl;			
				output << "====================" << std::endl;
				output << data.status();
				output << taskManager.status();
				
				
				// Get status of controller
				sendDalilahAnswer( p->message.command().sender_id() , fromIdentifier , false , true, output.str() );
				return;
			}
			
			// Try to schedule the command
			bool success = taskManager.addTask( fromIdentifier ,  p->message.command().command() , output  );
			
			// Send something back to dalilah ( if error -> it is also finish )
			sendDalilahAnswer( p->message.command().sender_id() , fromIdentifier , !success , !success,  output.str() );
			
			return;
		}
		
		// A confirmation from a worker is received
		
		if( p->message.type() == network::Message_Type_WorkerTaskConfirmation )
		{
			
			size_t task_id = p->message.worker_task_confirmation().task_id();
			int worker_id = fromIdentifier;
			taskManager.notifyWorkerConfirmation( task_id , worker_id );			
		}
		
	}

	void SamsonController::notificationSent( size_t id , bool success )
	{
		// Do something
	}
	
	
}
