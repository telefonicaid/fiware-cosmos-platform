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


	void* run_runStatusUpdate(void *p)
	{
		((SamsonWorker *)p)->runStatusUpdate();
		return NULL;
	}


/* ****************************************************************************
*
* Constructor
*/
SamsonWorker::SamsonWorker( NetworkInterface* network ) :  taskManager(this) , dataBuffer(this), loadDataManager(this)
{

	this->network = network;
	network->setPacketReceiverInterface(this);

	srand( (unsigned int) time(NULL) );
	
	// Create a thread to run "runStatusUpdate"
	pthread_t t;
	pthread_create(&t, NULL, run_runStatusUpdate, this);
	
}

/* ****************************************************************************
*
* run - 
*/
void SamsonWorker::runStatusUpdate()
{
	// Report periodically status to the controller
	
	while (true)
	{
		if( network->ready() )
			sendWorkerStatus();
		sleep(3);
	}
	
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
	loadDataManager.fill( ws );

	
	ws->set_used_memory( MemoryManager::shared()->get_used_memory() );
	
	network->send(this, network->controllerGetIdentifier(), Message::WorkerStatus, p);
}

		

	
	

int SamsonWorker::receive(int fromId, Message::MessageCode msgCode, Packet* packet)
{
	
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
