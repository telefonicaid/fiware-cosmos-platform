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
#include "WorkerDataManager.h"	// ss::WorkerDataManager
#include "ModulesManager.h"		// ss::ModulesManager
#include "WorkerTaskManager.h"	// ss::WorkerTaskManager
#include "DataBuffer.h"			// ss::DataBuffer



namespace ss {
	
	class EndpointMgr;
	class ProcessAssistant;

class SamsonWorker : public PacketReceiverInterface, public PacketSenderInterface
{
	friend class WorkerTaskManager;
	friend class WorkerDataManager;

public:
//	SamsonWorker(int argc, const char* argv[], NetworkInterface* _network);
	SamsonWorker(int argc, const char* argv[]);

	// command line argument variables
	int          port;
	int          endpoints;
	int          workers;
	std::string  controller;
	std::string  traceV;


private:
	EndpointMgr*        epMgr;             // Endpoint Manager
	NetworkInterface*   network;           // Network interface
	WorkerDataManager   data;              // Data manager
	WorkerTaskManager   taskManager;       // Task manager
	ModulesManager      modulesManager;    // Manager of the modules we have
	ProcessAssistant**  processAssistant;
	DataBuffer dataBuffer;				// Element used to buffer incomming data packets before they are joined and saved to disk
		


private:
	void parseArgs(int argC, const char* argV[]);
	void logInit(const char* pName);

private:
	Message::WorkerStatusData status;

public:
	void networkSet(NetworkInterface* network);
	void endpointMgrSet(ss::EndpointMgr* epMgr);
	

	// Main routine
	void run();
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
