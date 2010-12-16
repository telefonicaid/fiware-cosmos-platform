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

#include "MemoryManager.h"				// ss::SharedMemory
#include "FileManager.h"				// ss::FileManager

namespace ss {

	

/* ****************************************************************************
*
* run_thread_sending_worker_status - 
*/
void* run_thread_sending_worker_status(void* p)
{
	SamsonWorker* worker = (SamsonWorker*) p;

	while (true)
	{
		worker->sendWorkerStatus();
		sleep(3);
	}

	return NULL;
}
	


/* ****************************************************************************
*
* Constructor
*/
SamsonWorker::SamsonWorker(char* controller, char* alias, unsigned short port, int workers, int endpoints) :  taskManager(this) , dataBuffer(this), loadDataManager(this)
{
	this->controller  = controller;
	this->alias       = alias;
	this->port        = port;
	this->workers     = SamsonSetup::shared()->num_workers;
	this->endpoints   = endpoints;

	srand( (unsigned int) time(NULL) );

	// Setup of the run-time status
	setStatusTile( "Samson Worker" , "worker" );
	addChildrenStatus( MemoryManager::shared() );
	addChildrenStatus( &taskManager );
	addChildrenStatus( &dataBuffer );
	addChildrenStatus( FileManager::shared() );
	addChildrenStatus( DiskManager::shared() );
	addChildrenStatus( &loadDataManager );
	//addChildrenStatus( network );
	
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
	
	// Get my id as worker ( could be -1 )
	_myWorkerId = network->getWorkerFromIdentifier(network->getMyidentifier());

	if (_myWorkerId == -1)
	   LM_X(1, ("alias: '%s' (network->getMyidentifier returns %d)", alias.c_str(), network->getMyidentifier()));

	this->workers     = network->getNumWorkers();
	
}



/* ****************************************************************************
*
* run - 
*/
void SamsonWorker::run()
{
	// Start a thread to report status to the controller in a regular basis
	pthread_t t;
	pthread_create(&t, 0, run_thread_sending_worker_status, this);
	
	assert(network);
	network->run();
}




void SamsonWorker::sendWorkerStatus()
{
	Packet *p = new Packet();
	network::WorkerStatus*  ws = p->message.mutable_worker_status();
	
	// Fill to all data related with task manager
	taskManager.fill(ws);
	
	// Fill information related with file manager and disk manager
	DiskManager::shared()->fill( ws );
	FileManager::shared()->fill( ws );
	MemoryManager::shared()->fill( ws );
	
	dataBuffer.fill( ws );
	
	network->send(this, network->controllerGetIdentifier(), Message::WorkerStatus, p);
}

		

	
	

int SamsonWorker::receive(int fromId, Message::MessageCode msgCode, Packet* packet)
{
	
	if (msgCode == Message::StatusRequest)
	{
		Packet* p = new Packet();

		network::StatusResponse *response = p->message.mutable_status_response();

		
		response->set_title( "Worker " + au::Format::string( network->getWorkerFromIdentifier( network->getMyidentifier() ) ) );

		response->set_senderid( packet->message.status_request().senderid() ) ;

		response->set_response( getStatus( packet->message.status_request().command() ) );
		
		network->send(this, network->controllerGetIdentifier() , Message::StatusResponse, p);

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
	if (msgCode == Message::UploadData)
	{
		std::ostringstream fileName;
		fileName << "/tmp/file_"<< getpid() << "_" << rand() << rand();	// Just to test
		
		loadDataManager.addUploadItem(fromId, packet->message.upload_data(), packet->message.delilah_id() , packet->buffer );
		return 0;
	}

	// Download data files
	if (msgCode == Message::DownloadData)
	{
		loadDataManager.addDownloadItem(fromId, packet->message.download_data() , packet->message.delilah_id() );
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
			network::Queue queue = packet->message.data().queue();
			
			bool txt = false;
			if( packet->message.data().has_txt() && packet->message.data().txt() )
				txt = true;
			
			dataBuffer.addBuffer(task_id, queue, packet->buffer , txt );
			
			return 0;
		}

		/** 
		 Data Close message is sent to notify that no more data will be generated
		 We have to wait for "N" messages ( one per worker )
		 */
	
		if( msgCode == Message::WorkerDataExchangeClose )
		{
			
			size_t task_id = packet->message.data_close().task_id();
			dataBuffer.finishWorker( task_id );
			return 0;
		}
	

		return 0;
	}

}
