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
#include "CommandLine.h"		// au::CommandLine
#include "ModulesManager.h"		// ss::ModulesManager
#include "WorkerTaskManager.h"	// ss::WorkerTaskManager
#include "DataBuffer.h"			// ss::DataBuffer
#include "LoadDataManager.h"	// ss::LoadDataManager
#include "Status.h"				// au::Status

namespace ss {
	
	class EndpointMgr;
		
	class SamsonWorker : public PacketReceiverInterface, public PacketSenderInterface , public au::Status
	{
		
	public:
		SamsonWorker(char* controller, char* alias, unsigned short port, int workers, int endpoints);

		
		// command line argument variables
		std::string     controller;
		std::string     alias;
		unsigned short  port;
		int             endpoints;
		int             workers;
		
		int             workersGet(void)        { return workers; }

	public:

		EndpointMgr*         epMgr;             // Endpoint Manager
		NetworkInterface*    network;           // Network interface
		ModulesManager       modulesManager;    // Manager of the modules we have
		WorkerTaskManager    taskManager;       // Task manager
		DataBuffer           dataBuffer;        // Element used to buffer incoming data packets before they are joined and saved to disk
		LoadDataManager      loadDataManager;   // Element used to save incoming txt files to disk ( it waits until finish and notify delilah )
		int                  _myWorkerId;        // My id as worker : 0 , 1 ,2 ,3

	public:
		void networkSet(NetworkInterface* network);
		void endpointMgrSet(ss::EndpointMgr* epMgr);


		// Main routines
		void run();
		void test();

		// PacketReceiverInterface
		virtual int receive(int fromId, Message::MessageCode msgCode, Packet* packet);

		// Send information about the state of this worker to the controller
		void sendWorkerStatus();
		

		int getWorkerId()
		{
			if( _myWorkerId == -1)
				_myWorkerId = network->getWorkerFromIdentifier(network->getMyidentifier());
			
			return _myWorkerId;
		}
		
		private:
		virtual void notificationSent(size_t id, bool success) {}
		
	};
}

#endif
