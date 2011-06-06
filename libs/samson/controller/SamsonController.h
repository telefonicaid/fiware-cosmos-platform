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

#include "logMsg/logMsg.h"                     // lmInit, LM_*

#include "samson/common/Macros.h"                     // EXIT, ...
#include "samson/network/Network.h"                    // NetworkInterface
#include "samson/network/Endpoint.h"                   // Endpoint
#include "au/CommandLine.h"                // au::CommandLine
#include "samson/common/samsonDirectories.h"          // File to load setup
#include "ControllerDataManager.h"      // samson::ControllerDataManager
#include "samson/module/ModulesManager.h"             // samson::ModulesManager
#include "ControllerTaskManager.h"      // samson::ControllerTaskManager
#include "samson/common/samson.pb.h"                  // network::...
#include "JobManager.h"                 // samson::JobManager
#include "Monitor.h"                    // samson::Monitor
#include "ControllerLoadManager.h"		// samson::ControllerLoadManager
#include "engine/EngineElement.h"				// samson::EngineElement
#include "engine/EngineNotification.h"         // samson::EngineNotificationListener
#include "au/Cronometer.h"              // au::Cronometer
#include "samson/network/NetworkNode.h" // samson::NetworkNode

#define notification_monitorization                 "notification_monitorization"

namespace samson {
	
	/**
	 Main class for Samson Controller
	 */
	
	class SamsonController : public PacketReceiverInterface , public PacketSenderInterface , public engine::NotificationListener
	{
        // Network interface used by this element
        NetworkInterface*  network;
        
		// Initial time stamp 
		struct timeval init_time;
		
		// Data manager for the controller
		ControllerDataManager data;							
		
		// Top level job manager
		JobManager jobManager;								

		// Monitorization control for web-based moitoring tool
		friend class SamsonControllerMonitor;
		Monitor monitor;									
		
		// Load manager
		ControllerLoadManager loadManager;
		
		// Status information of the workers
		network::WorkerStatus** worker_status;				// Status of the workers reported periodically
        au::Cronometer *worker_status_cronometer;           // Cronometer to count the last update from workers
		int num_workers;
				
		friend class Job;
		friend class JobManager;
		friend class ControllerDataManager;
		friend class Monitor;
		friend class ControllerTaskManager;
		friend class ControllerTask;

	public:
		
		SamsonController( NetworkInterface* network );
		~SamsonController();
				
		// PacketReceiverInterface
		void receive( Packet* packet );
		void notifyWorkerDied( int worker );
		
		// PacketSenderInterface
		virtual void notificationSent( size_t id , bool success );

		virtual std::string getJSONStatus(std::string in)
		{
			return monitor.getJSONString( in );
		}
	
		void fill( network::ControllerStatus *status );
		
		void pushSystemMonitor( MonitorBlock  *);
        
        // Notifications
        void notify( engine::Notification* notification );
        bool acceptNotification( engine::Notification* notification );
		
	};
	
	
}

#endif
