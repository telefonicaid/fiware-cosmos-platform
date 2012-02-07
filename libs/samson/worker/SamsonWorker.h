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

#include "samson/stream/StreamManager.h"        // samson::stream::StreamManager
#include "samson/stream/QueueTaskManager.h"     // samson::stream::QueueTaskManager

#include "samson/common/traces.h"				// Trace levels
#include "samson/common/Macros.h"				// exit(.)
#include "samson/common/samsonDirectories.h"  // SAMSON_WORKER_DEFAULT_PORT
#include "samson/common/samson.pb.h"			// samson::network::

#include "samson/data/SimpleDataManager.h"          // samson::SimpleDataManager

#include "samson/common/NotificationMessages.h"

#include "samson/network/NetworkInterface.h"

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
        public NetworkInterfaceReceiver, 
        public engine::Object
	{
		
		// Initial time stamp 
		struct timeval init_time;
        
	public: 
		
		SamsonWorker( NetworkInterface* network );
        
        friend class StreamManager;                     // Friend to be able to log to dataManager
        
	public:

        NetworkInterface *network;                      // Network interface to communicate with the rest of the system
				
        stream::StreamManager* streamManager;           // Manager of all the stream-processing queues in the system
        
        std::list < WorkerLog > activityLog;            // Activity log for this worker
        
	public:

		// NetworkInterfaceReceiver
        // ----------------------------------------------------------------
		void receive( Packet* packet );
        std::string getRESTInformation( ::std::string in );
        void reset_worker( size_t worker_id );
		
        // Notification from the engine about finished tasks
        void notify( engine::Notification* notification );

        // Get information for monitoring
        void getInfo( std::ostringstream& output);
        
        // Log activity
        void logActivity( std::string log);

	};
	
}

#endif
