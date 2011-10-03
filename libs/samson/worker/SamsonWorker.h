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


#include "engine/EngineElement.h"               // samson::EngineElement

#include "samson/network/NetworkNode.h"     // samson::NetworkNode

#include "samson/stream/StreamManager.h"        // samson::stream::StreamManager
#include "samson/stream/QueueTaskManager.h"     // samson::stream::QueueTaskManager

#include "samson/common/traces.h"				// Trace levels
#include "samson/common/Macros.h"				// exit(.)
#include "samson/common/samsonDirectories.h"  // SAMSON_WORKER_DEFAULT_PORT
#include "samson/common/samson.pb.h"			// samson::network::


#include "WorkerTaskManager.h"	// samson::WorkerTaskManager
#include "LoadDataManager.h"	// samson::LoadDataManager

#include "samson/data/SimpleDataManager.h"          // samson::SimpleDataManager

#include "samson/common/NotificationMessages.h"

namespace samson {
	
    class NetworkInterface;
    class Info;
    
    class WorkerLog
    {
        
    public:
        
        WorkerLog( std::string _txt );
        
        std::string time; 
        std::string txt;
        
        void getInfo( std::ostringstream & output );
        
    };
    
	class SamsonWorker : 
        public PacketReceiverInterface, 
        public PacketSenderInterface, 
        public engine::Object
	{
		
		// Initial time stamp 
		struct timeval init_time;
        
	public: 
		
		SamsonWorker(NetworkInterface* network);
        
        friend class StreamManager;                     // Friend to be able to log to dataManager
        
	public:

        NetworkInterface *network;                      // Network interface to communicate with the rest of the system
		
		WorkerTaskManager taskManager;                  // Task manager for batch processing operations
        
		LoadDataManager loadDataManager;                // Element used to save incoming txt files to disk ( it waits until finish and notify delilah )
		
        stream::StreamManager streamManager;            // Manager of all the stream-processing queues in the system
        
        std::list < WorkerLog > activityLog;          // Activity log for this worker
        
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

        // Get information for monitorization
        void getInfo( std::ostringstream& output);
        
        
        // Log activity
        void logActivity( std::string log);
        
	private:
		
		virtual void notificationSent(size_t id, bool success) {}
		
		// Sent an "ls" to get the list of files ( to remove the rest )
		void sendFilesMessage();
        

	};
	
}

#endif
