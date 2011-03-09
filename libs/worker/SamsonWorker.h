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

namespace ss {
	
	class SamsonWorker : public PacketReceiverInterface, public PacketSenderInterface
	{
		
	public:
		
		SamsonWorker(NetworkInterface* network);

	public:

		NetworkInterface*    network;           // Network interface
		WorkerTaskManager    taskManager;       // Task manager
		LoadDataManager      loadDataManager;   // Element used to save incoming txt files to disk ( it waits until finish and notify delilah )
		int                  _myWorkerId;       // My id as worker : 0 , 1 ,2 ,3

	public:

		// PacketReceiverInterface
		virtual int receive(int fromId, Message::MessageCode msgCode, Packet* packet);
		
		// Send information about the state of this worker to the controller
		void sendWorkerStatus();
		
		// Sent status messages to the controllor periodically
		void runStatusUpdate();
		
		// Nothing function to avoid warning
		void touch(){};	
		
		// Process list of files ( to remove unnecessary files )
		void processListOfFiles( const network::QueueList& ql);
		
	private:
		
		virtual void notificationSent(size_t id, bool success) {}
		
	};
}

#endif
