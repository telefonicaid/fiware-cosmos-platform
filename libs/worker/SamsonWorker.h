#ifndef SAMSON_WORKER_H
#define SAMSON_WORKER_H

/* ****************************************************************************
*
* FILE                     SamsonWorker.h
*
* DESCRIPTION			   Main class for the worker element
*
*/

#include <iostream>				// std::cout

#include "logMsg.h"				// 
#include "traces.h"				// Trace levels
#include "Macros.h"				// exit(.)
#include "Network.h"			// NetworkInterface
#include "samsonDirectories.h"  // SAMSON_WORKER_DEFAULT_PORT
#include "workerStatus.h"		// WorkerStatus
#include "ModulesManager.h"		// ss::ModulesManager
#include "WorkerTaskManager.h"	// ss::WorkerTaskManager
#include "DataBuffer.h"			// ss::DataBuffer
#include "LoadDataManager.h"	// ss::LoadDataManager
#include "samson.pb.h"			// ss::network::
#include "EngineElement.h"		// ss::EngineElement

namespace ss {
	
	class SamsonWorker : public PacketReceiverInterface, public PacketSenderInterface
	{
		
	public:
		
		SamsonWorker(NetworkInterface* network);

	public:

		NetworkInterface*    network;           // Network interface to send packets
		
		WorkerTaskManager    taskManager;       // Task manager
		LoadDataManager      loadDataManager;   // Element used to save incoming txt files to disk ( it waits until finish and notify delilah )
		
		int                  _myWorkerId;       // My id as worker : 0 , 1 ,2 ,3

	public:

		// PacketReceiverInterface
		void receive( Packet* packet );
		
		// Send information about the state of this worker to the controller
		void sendWorkerStatus();
		
		// Sent an "ls" to get the list of files ( to remove the rest )
		void sendFilesMessage();
		
		// Nothing function to avoid warning
		void touch(){};	
		
		// Process list of files ( to remove unnecessary files )
		void processListOfFiles( const network::QueueList& ql);
		
	private:
		
		virtual void notificationSent(size_t id, bool success) {}
		
	};
	
	
	// Class to run the status update
	
	class SamsonWorkerStatusUpdater : public EngineElement
	{
		SamsonWorker * samsonWorker;
		
	public:
		
		SamsonWorkerStatusUpdater( SamsonWorker * _samsonWorker ) : EngineElement( 3 )
		{
			samsonWorker = _samsonWorker;
			description = "SamsonWorkerStatusUpdater";
		}
		
		void run()
		{
			// Send the status updater message
			samsonWorker->sendWorkerStatus();
		}
		
	};

	// Class to send an "ls" every 3 seconds to control files to be removed at the response
	
	class SamsonWorkerFileUpdater : public EngineElement
	{
		SamsonWorker * samsonWorker;
		
	public:
		
		SamsonWorkerFileUpdater( SamsonWorker * _samsonWorker ) : EngineElement( 3 )
		{
			samsonWorker = _samsonWorker;
			description = "SamsonWorkerFileUpdater";
		}
		
		void run()
		{
			// Send the status updater message
			samsonWorker->sendFilesMessage();
		}
		
	};
	
	
}

#endif
