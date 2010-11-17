#include <iostream>                     // std::cout ...

#include "logMsg.h"                     // lmInit, LM_*
#include "coreTracelevels.h"            // LMT_*

#include "Message.h"                    // Message
#include "Macros.h"                     // EXIT, ...
#include "Packet.h"                     // ss::Packet
#include "Network.h"                    // NetworkInterface
#include "Endpoint.h"                   // Endpoint
#include "CommandLine.h"                // CommandLine
#include "ProcessAssistant.h"           // ProcessAssistant
#include "SamsonWorker.h"               // Own interfce
#include "EndpointMgr.h"				// ss::EndpointMgr
#include "SamsonSetup.h"				// ss::SamsonSetup
#include "Format.h"						// au::Format
		
namespace ss {



/* ****************************************************************************
*
* Constructor
*/
SamsonWorker::SamsonWorker(char* controller, char* alias, unsigned short port, int workers, int endpoints) :  taskManager(this) , loadDataManager(this)
{
	this->controller  = controller;
	this->alias       = alias;
	this->port        = port;
	this->workers     = workers;
	this->endpoints   = endpoints;

	// Define the number of process
	num_processes = SamsonSetup::shared()->getInt( SETUP_num_processes , -1);
	
}



/* ****************************************************************************
*
* endpointMgrSet - 
*/
void SamsonWorker::endpointMgrSet(ss::EndpointMgr* _epMgr)
{
	epMgr = _epMgr;

	// epMgr->init(Endpoint::Worker, alias.c_str(), port, controller.c_str());
	// epMgr->packetReceiverSet(this);
}



/* ****************************************************************************
*
* networkSet - 
*/
void SamsonWorker::networkSet(NetworkInterface* network)
{
	this->network = network;
	network->setPacketReceiverInterface(this);

	network->init(Endpoint::Worker, alias.c_str(), port, controller.c_str());
	
	// Get my id as worker
	myWorkerId = network->getWorkerFromIdentifier(network->getMyidentifier());
	LM_M(("My worker id: %d", myWorkerId));
}



/* ****************************************************************************
*
* run - 
*/
void SamsonWorker::run()
{
	workerStatus(&status);

	

	// //////////////////////////////////////////////////////////////////////
	//
	// Create one ProcessAssistant per core
	//
	if( num_processes == -1)
		LM_X( 1 ,("Invalid number of cores. Please edit /opt/samson/setup.txt.")  );
	
	LM_T(LMT_WINIT, ("initializing %d process assistants", num_processes));

	processAssistant = (ProcessAssistant**) calloc(num_processes, sizeof(ProcessAssistant*));
	if (processAssistant == NULL)
		LM_XP(1, ("calloc(%d, %d)", num_processes, sizeof(ProcessAssistant*)));

	int coreId;
	for (coreId = 0; coreId < num_processes ; coreId++)
		processAssistant[coreId] = new ProcessAssistant(coreId, controller.c_str(), this);

	LM_T(LMT_WINIT, ("Got %d process assistants", coreId));


	
	// assert(epMgr);
	// epMgr->run();

	assert(network);
	network->run();
}



#if 0
void SamsonWorker::sendWorkerStatus()
{
	Packet                  p;
	network::WorkerStatus*  w = p.message.mutable_worker_status();
	
	// Fill with all data related to data
	data.fillWorkerStatus(w);
	
	// Fill to all data related with task manager
	taskManager.fillWorkerStatus(w);
	
	network->send(this, network->controllerGetIdentifier(), Message::WorkerStatus, NULL, 0, &p);
}
#endif
		

	
	

int SamsonWorker::receive(int fromId, Message::MessageCode msgCode, Packet* packet)
{
	
	if (msgCode == Message::StatusRequest)
	{
		Packet p;
		network::StatusResponse *response = p.message.mutable_status_response();
		response->set_title( "Worker " + network->getWorkerFromIdentifier( network->getMyidentifier() ) );
		response->set_response( getStatus( packet->message.status_request().command() ) );
		network->send(this, fromId, Message::StatusResponse, &p);
		return 0;
	}

	
	if (msgCode == Message::WorkerTask)
	{
		// A packet with a particular command is received (controller expect to send a confirmation message)
		LM_T(LMT_TASK, ("Got a WorkerTask message"));
		
		// add task to the task manager
		taskManager.addTask( packet->message.worker_task() );
		return 0;
	}

	// Load data files to be latter confirmed to controller
	if (msgCode == Message::LoadData)
	{
		
		std::ostringstream fileName;
		fileName << "/tmp/file_" << rand() << rand();	// Just to test
		
		loadDataManager.addFile( packet->buffer , fileName.str() , fromId , packet->message.load_data().process_id() , packet->message.load_data().file_id() );
		return 0;
	}
	
	
	
		/**
		 Data packets go directly to the DataBuffer to be joined and flushed to disk
		 DataManager is notifyed when created a new file or finish everything 
		 */
		
		if( msgCode == Message::WorkerDataExchange )
		{
			// New data packet for a particular queue inside a particular task environment
		
			size_t task_id = packet->message.data().task_id();
			std::string queue = packet->message.data().queue();
			
			dataBuffer.addBuffer(task_id, queue, packet->buffer );
			
			return 0;
		}

		return 0;
	}

	std::string SamsonWorker::getStatus(std::string command)
	{
		std::ostringstream output;
		output << "** Memory: " << MemoryManager::shared()->getStatus() << std::endl;

		output << "** TaskManager:\n";
		output << taskManager.getStatus();

		output << "** ProcessAssistants: (" << num_processes << " process):\n";
		for (int i = 0 ; i < num_processes ; i++)
			output << "\t" << processAssistant[i]->getStatus() << std::endl; 

		output << "** DataBuffer:\n";
		output << dataBuffer.getStatus();

		output << "** LoadManager:\n";
		output << loadDataManager.getStatus();
		
		
		return output.str();
		
	}


}
