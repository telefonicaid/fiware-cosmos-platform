#ifndef SAMSON_CONTROLLER_H
#define SAMSON_CONTROLLER_H

/* ****************************************************************************
*
* FILE                      SamsonController.h
*
* DESCRIPTION				Main class for the controller
*
*/
#include <iostream>                     // std::cout

#include "logMsg.h"                     // lmInit, LM_*

#include "Macros.h"                     // EXIT, ...
#include "Network.h"                    // NetworkInterface
#include "Endpoint.h"                   // Endpoint
#include "CommandLine.h"                // au::CommandLine
#include "samsonDirectories.h"          // File to load setup
#include "ControllerDataManager.h"      // ss::ControllerDataManager
#include "ModulesManager.h"             // ss::ModulesManager
#include "ControllerTaskManager.h"      // ss::ControllerTaskManager
#include "samson.pb.h"                  // network::...
#include "workerStatus.h"               // Message::WorkerStatusData
#include "Message.h"                    // Message::WorkerStatus, ...
#include "JobManager.h"                 // ss::JobManager
#include "Monitor.h"                    // ss::Monitor
#include "Status.h"                     // au::Status
#include "ControllerLoadManager.h"		// ss::ControllerLoadManager


namespace ss {
	
	/**
	 Main class for Samson Controller
	 */
	
	class SamsonController : public PacketReceiverInterface , public PacketSenderInterface 
	{
		// Elements inside the SamsonController
		
		// Network interface
		NetworkInterface*     network;						

		// Data manager for the controller
		ControllerDataManager data;							
		
		// Top level job manager
		JobManager jobManager;								

		// Monitorization control for web-based moitoring tool
		Monitor monitor;									
		
		// Load manager
		ControllerLoadManager loadManager;
		
		// Status information of the workers
		network::WorkerStatus** worker_status;				// Status of the workers reported periodically
		struct timeval *worker_status_time;					// Last time status was reported
		au::Lock worker_status_lock;						// Lock to protect this
		int num_workers;
				
		friend class ControllerTaskManager;
		friend class Job;
		friend class JobManager;
		friend class ControllerDataManager;
		friend class Monitor;

	public:
		
		SamsonController(NetworkInterface* network);
		
		void runBackgroundProcesses();
		
		// PacketReceiverInterface
		int receive(int fromId, Message::MessageCode msgCode, Packet* packet);
		void notifyWorkerDied( int worker );
		
		// Specialized receive functions
		int receiveHelp(int fromId, Packet* packet);
		
		// PacketSenderInterface
		virtual void notificationSent( size_t id , bool success );

		virtual std::string getJSONStatus(std::string in)
		{
			return monitor.getJSONString( in );
		}
	
		// Nothing function to avoid warning
		void touch(){};	

		void fill( network::ControllerStatus *status );
		
		void pushSystemMonitor( MonitorBlock  *);
		
		void runAutomaticOperationThread();
		
	};
	
}

#endif
