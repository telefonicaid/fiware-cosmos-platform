#include <iostream>				// std::cout ...

#include "logMsg.h"             // lmInit, LM_*
#include "Macros.h"             // EXIT, ...
#include "Packet.h"				// ss::Packet
#include "Network.h"			// NetworkInterface
#include "Endpoint.h"			// Endpoint
#include "CommandLine.h"		// CommandLine
#include "SamsonWorker.h"		// Own interfce
#include "traces.h"				// Trace definition
#include "WorkerDataManager.h"

namespace ss {


	SamsonWorker::SamsonWorker(int argc, const char* argv[] , NetworkInterface *_network) : data( this ) , taskManager( this )
	{
		network = _network;
		network->setPacketReceiverInterface(this);
		
		int          port;
		std::string  controller;
		std::string  trace;
		
		// Parse input command lines
		au::CommandLine commandLine;
		commandLine.parse(argc, argv);
		
		commandLine.set_flag_int("port",           SAMSON_WORKER_DEFAULT_PORT);
		commandLine.set_flag_string("controller" , "no_controller");
		commandLine.set_flag_string("t",           "255");
		commandLine.set_flag_boolean("r");
		commandLine.set_flag_boolean("w");
		
		commandLine.parse(argc, argv);
		
		port       = commandLine.get_flag_int("port");
		controller = commandLine.get_flag_string("controller");
		lmReads    = commandLine.get_flag_bool("r");
		lmWrites   = commandLine.get_flag_bool("w");
		
		if (controller == "no_controller")
		{
			std::cerr  << "Please specify controller direction with -controller server:port" << std::endl;
			exit(0);
		}
		
		LM_T( TRACE_SAMSON_WORKER , ("Samson worker running at port %d controller: %s", port , controller.c_str() ));
		
		network->initAsSamsonWorker( port , controller );
	}	
	
	void SamsonWorker::run()
	{
		// Init the data manager
		data.initDataManager( data.getLogFileName() );
		
		assert( network );
		network->run();
	}

	void SamsonWorker::sendWorkerStatus()
	{
		Packet p( network::Message_Type_WorkerStatus );
		network::WorkerStatus * w =p.message.mutable_worker_status();
		
		// Fill with all data related to data
		data.fillWorkerStatus( w );
		
		// Fill to all data related with task manager
		taskManager.fillWorkerStatus( w );
		
		network->send(&p, network->controllerGetIdentifier(), this);
		
	}
		
	void SamsonWorker::sentConfirmationToController(size_t task_id )
	{
		Packet p( network::Message_Type_WorkerTaskConfirmation );
		network::WorkerTaskConfirmation * confirmation = p.message.mutable_worker_task_confirmation();
		confirmation->set_task_id( task_id );
		confirmation->set_error( false );
		network->send(&p, network->controllerGetIdentifier(), this);
	}
	
	
	void SamsonWorker::receive(Packet* p, int from)
	{
		
		if( p->message.type() == network::Message_Type_WorkerTask )
		{
			// A packet with a particular command is received ( controller expect to send a confirmation message )
			
			// Process the command in the data manager ( rigth now this is just to test )
			data.beginTask( p->message.worker_task().task_id() );
			
			data.runOperationOfTask(p->message.worker_task().task_id() , p->message.worker_task().command() );

			if( taskManager.addTask( p->message.worker_task() ) )
			{
				data.finishTask( p->message.worker_task().task_id() );
			
				// Sent a confirmation just to test everything is ok
				sentConfirmationToController( p->message.worker_task().task_id() );
				
				// Send a update
				sendWorkerStatus();
				
			}
				
		}
		
	}

	void SamsonWorker::notificationSent( size_t id , bool success )
	{
		// Do something
	}

}
