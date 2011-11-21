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

#include "au/CommandLine.h"                // au::CommandLine
#include "au/Cronometer.h"              // au::Cronometer

#include "engine/Object.h"         // samson::EngineNotificationListener
#include "engine/Object.h"                  // engine::Object

#include "samson/common/Macros.h"                     // EXIT, ...
#include "samson/common/samsonDirectories.h"          // File to load setup
#include "samson/module/ModulesManager.h"             // samson::ModulesManager

#include "samson/network/Network.h"                    // NetworkInterface
#include "samson/network/Endpoint.h"                   // Endpoint
#include "samson/network/NetworkNode.h" // samson::NetworkNode

#include "ControllerDataManager.h"      // samson::ControllerDataManager
#include "ControllerTaskManager.h"      // samson::ControllerTaskManager
#include "samson/common/samson.pb.h"                  // network::...

#include "JobManager.h"                 // samson::JobManager
#include "Monitor.h"                    // samson::Monitor
#include "ControllerLoadManager.h"		// samson::ControllerLoadManager
#include "engine/EngineElement.h"				// samson::EngineElement

#define notification_monitoring                 "notification_monitoring"

namespace samson {
	
    
	/**
	 Main class for Samson Controller
	 */
	
	class SamsonController : public PacketReceiverInterface , public PacketSenderInterface , public engine::Object
	{
        // Network interface used by this element
        NetworkInterface*  network;
        
		// Initial time stamp 
		struct timeval init_time;
		
		// Data manager for the controller
		ControllerDataManager data;							

		// Load manager
		ControllerLoadManager loadManager;
		
		// Top level job manager
		JobManager jobManager;								

		// Monitoring control for web-based monitoring tool
		friend class SamsonControllerMonitor;
		Monitor monitor;									
		
		// Status information of the workers
        std::string* worker_xml_info;                       // XML version of the worker status information
        au::Cronometer *cronometer_worker_xml_info;         // Cronometer to count the last update from workers
		
        int num_workers;

        // Unified information ( updated with information coming from workers )
        
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
		
		// PacketSenderInterface
		virtual void notificationSent( size_t id , bool success );

		virtual std::string getJSONStatus(std::string in)
		{
			return monitor.getJSONString( in );
		}
		
		void pushSystemMonitor( MonitorBlock  *);
        
        // Notifications
        void notify( engine::Notification* notification );

        // get xml information
        void getInfo( std::ostringstream& output);
        
        
	};
	
	
}

#endif
