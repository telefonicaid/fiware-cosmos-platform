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

#include "logMsg/logMsg.h"				// 
#include "samson/common/traces.h"				// Trace levels
#include "samson/common/Macros.h"				// exit(.)
#include "samson/common/samsonDirectories.h"  // SAMSON_WORKER_DEFAULT_PORT
#include "WorkerTaskManager.h"	// samson::WorkerTaskManager
#include "LoadDataManager.h"	// samson::LoadDataManager
#include "samson/common/samson.pb.h"			// samson::network::
#include "engine/EngineElement.h"               // samson::EngineElement

#include "samson/network/NetworkNode.h"     // samson::NetworkNode

#include "samson/stream/QueuesManager.h"        // samson::stream::QueuesManager
#include "samson/stream/QueueTaskManager.h"     // samson::stream::QueueTaskManager

#define notification_samson_worker_send_status_update "notification_samson_worker_send_status_update"
#define notification_samson_worker_send_trace "notification_samson_worker_send_trace"
#define notification_send_to_worker "notification_send_to_worker"

namespace samson {
	
    class NetworkInterface;
    
	class SamsonWorker :  public PacketReceiverInterface, public PacketSenderInterface, public engine::NotificationListener
	{
		
		// Initial time stamp 
		struct timeval init_time;
        
	public: 
		
		SamsonWorker(NetworkInterface* network);
        
        
	public:

        NetworkInterface *network;
		
		WorkerTaskManager    taskManager;               // Task manager
		LoadDataManager      loadDataManager;           // Element used to save incoming txt files to disk ( it waits until finish and notify delilah )
		
        stream::QueuesManager   queuesManager;          // Manager of all the stream-processing queues in the system
        
	public:

		// PacketReceiverInterface
		void receive( Packet* packet );
		
		// Send information about the state of this worker to the controller
		void sendWorkerStatus();
		
		// Nothing function to avoid warning
		void touch(){};	
		
		// Process list of files ( to remove unnecessary files )
		void processListOfFiles( const network::QueueList& ql);
		
        // Notification from the engine about finished tasks
        void notify( engine::Notification* notification );
        bool acceptNotification( engine::Notification* notification );

        
	private:
		
		virtual void notificationSent(size_t id, bool success) {}
		
		// Sent an "ls" to get the list of files ( to remove the rest )
		void sendFilesMessage();
        
	};
	
}

#endif
