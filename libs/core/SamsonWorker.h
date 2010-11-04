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
#include "Endpoint.h"			// Endpoint
#include "CommandLine.h"		// au::CommandLine
#include "WorkerDataManager.h"	// ss::WorkerDataManager
#include "ModulesManager.h"		// ss::ModulesManager
#include "WorkerTaskManager.h"	// ss::WorkerTaskManager



namespace ss {
	
	/**
	 Main class for the samson worker element
	 */
	
	class SamsonWorker : public PacketReceiverInterface, public PacketSenderInterface
	{
		NetworkInterface*  network;			// Network interface
		WorkerDataManager  data;			// Data manager
		WorkerTaskManager  taskManager;		// Task manager
		ModulesManager     modulesManager;	// Manager of the modules we have
		
		friend class WorkerTaskManager;
		friend class WorkerDataManager;
		

		// command line argument variables
	public:
        int          port;
        int          endpoints;
        int          workers;
		std::string  controller;
		std::string  traceV;
		

	public:
		SamsonWorker(int argc, const char* argv[], NetworkInterface* _network);
		SamsonWorker(int argc, const char* argv[]);

	private:
		void parseArgs(int argC, const char* argV[]);
		void logInit(const char* pName);

	public:
		void networkSet(NetworkInterface* network);

		// Main routine
		void run();

		// Test routine
		void test();
		
		// PacketReceiverInterface
		virtual int receive(int fromId, Message::MessageCode msgCode, void* dataP, int dataLen, Packet* packet);

		// PacketSenderInterface
		virtual void notificationSent(size_t id, bool success);
		
		
	private:
		
#if 0
		/** 
		 Send a WorkerStatus message to controller
		 */
		
		void sendWorkerStatus();
#endif
		
		// send the confirmation of a particular task to the controller
		void sentConfirmationToController(size_t task_id );
		
	};
}

#endif
