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
		friend class ProcessWriter;
		
	public:
		SamsonWorker(char* controller, char* alias, unsigned short port, int workers, int endpoints);

		
		// command line argument variables
		std::string     controller;
		std::string     alias;
		unsigned short  port;
		int             endpoints;
		int             workers;

	private:
		EndpointMgr*         epMgr;             // Endpoint Manager
		NetworkInterface*    network;           // Network interface
		ModulesManager       modulesManager;    // Manager of the modules we have
		WorkerTaskManager    taskManager;       // Task manager
		ProcessAssistant**   processAssistant;  // vector of core worker processes
		DataBuffer           dataBuffer;        // Element used to buffer incoming data packets before they are joined and saved to disk
		LoadDataManager      loadDataManager;   // Element used to save incoming txt files to disk ( it waits until finish and notify delilah )
		int                  myWorkerId;        // My id as worker : 0 , 1 ,2 ,3
		
		int					 num_processes;		// Number of process assitants we will have

	private:
		Message::WorkerStatusData status;

	public:
		void networkSet(NetworkInterface* network);
		void endpointMgrSet(ss::EndpointMgr* epMgr);


		// Main routines
		void run();
		void test();


		// PacketReceiverInterface
		virtual int receive(int fromId, Message::MessageCode msgCode, Packet* packet);
			
		private:
		virtual void notificationSent(size_t id, bool success) {}
		
		
		//Internal functions to get help
		std::string getStatus(std::string command);
		
	};
}

#endif
