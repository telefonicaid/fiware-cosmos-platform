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

#include "Macros.h"				// EXIT
#include "Network.h"			// NetworkInterface
#include "samsonDirectories.h"  // SAMSON_WORKER_DEFAULT_PORT
#include "workerStatus.h"		// WorkerStatus
#include "CommandLine.h"		// au::CommandLine
#include "ModulesManager.h"		// ss::ModulesManager
#include "WorkerTaskManager.h"	// ss::WorkerTaskManager
#include "DataBuffer.h"			// ss::DataBuffer
#include "LoadDataManager.h"	// ss::LoadDataManager


namespace ss {
	
	class EndpointMgr;
	class ProcessAssistant;
		
	class SamsonWorker : public PacketReceiverInterface, public PacketSenderInterface
	{
		friend class WorkerTaskManager;
		friend class LoadDataManager;
		friend class ProcessAssistant;

	public:
		SamsonWorker(void);

		
		// command line argument variables
		int          port;
		int          endpoints;
		int          workers;
		std::string  controller;
		std::string  traceV;
		std::string  alias;

	private:
		EndpointMgr*        epMgr;             // Endpoint Manager
		NetworkInterface*   network;           // Network interface
		WorkerTaskManager   taskManager;       // Task manager
		ModulesManager      modulesManager;    // Manager of the modules we have
		ProcessAssistant**  processAssistant;
		DataBuffer dataBuffer;				// Element used to buffer incomming data packets before they are joined and saved to disk
		LoadDataManager loadDataManager;	// Element used to save incoming txt files to disk ( it waits until finish and notify delilah )

		int myWorkerId;		// My id as worker : 0 , 1 ,2 ,3
		

	public:
		void parseArgs(int argC, const char* argV[]);
		void logInit(const char*);

	private:
		Message::WorkerStatusData status;

	public:
		void networkSet(NetworkInterface* network);
		void endpointMgrSet(ss::EndpointMgr* epMgr);


		// Main routine
		void run();
		void test();


		// PacketReceiverInterface
		virtual int receive(int fromId, Message::MessageCode msgCode, Packet* packet);
			
		private:
			
	#if 0
		/** 
			Send a WorkerStatus message to controller
		*/
			
		void sendWorkerStatus();
	#endif

		
		virtual void notificationSent(size_t id, bool success)
		{
		}
		
		
	};
}

#endif
