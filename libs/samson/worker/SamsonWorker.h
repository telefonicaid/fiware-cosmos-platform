#ifndef SAMSON_WORKER_H
#define SAMSON_WORKER_H

/* ****************************************************************************
*
* FILE                     SamsonWorker.h
*
* DESCRIPTION			   Main class for the worker element
*
*/

#include <iostream>				                // std::cout

#include "logMsg/logMsg.h"				        // 

#include "au/Console.h"

#include "engine/EngineElement.h"               // samson::EngineElement

#include "samson/stream/StreamManager.h"        // samson::stream::StreamManager
#include "samson/stream/QueueTaskManager.h"     // samson::stream::QueueTaskManager

#include "samson/common/traces.h"				// Trace levels
#include "samson/common/Macros.h"				// exit(.)
#include "samson/common/samsonDirectories.h"    // SAMSON_WORKER_DEFAULT_PORT
#include "samson/common/samson.pb.h"			// samson::network::
#include "samson/common/NotificationMessages.h"

#include "samson/network/NetworkInterface.h"

#include "samson/worker/WorkerCommandManager.h"

namespace samson {
	
    class NetworkInterface;
    class Info;
    
	class SamsonWorker : 
        public NetworkInterfaceReceiver, 
        public engine::Object,
        public au::Console
	{
		// Initial time stamp 
		struct timeval init_time;
        
	public: 
		
		SamsonWorker( NetworkInterface* network );
        ~SamsonWorker()
        {
            LM_T(LmtCleanup, ("Deleting workerCommandManager: %p", workerCommandManager));
            delete workerCommandManager;

            LM_T(LmtCleanup, ("Deleting streamManager: %p", streamManager));
            delete streamManager;

            LM_T(LmtCleanup, ("Deleting network"));
            delete network;
        }

        friend class StreamManager;                     // Friend to be able to log to dataManager
        
	public:

        NetworkInterface *network;                      // Network interface to communicate with the rest of the system
				
        stream::StreamManager* streamManager;           // Manager of all the stream-processing queues in the system

        WorkerCommandManager* workerCommandManager;      // Manager of the "Worker commands"
        
        
	public:

		// NetworkInterfaceReceiver
        // ----------------------------------------------------------------
		void receive( Packet* packet );
        std::string getRESTInformation( ::std::string in );
		
        // Notification from the engine about finished tasks
        void notify( engine::Notification* notification );

        // Get information for monitoring
        void getInfo( std::ostringstream& output);
        
        
        // au::Console ( debug mode with fg )
        void evalCommand( std::string command );
        std::string getPrompt();

        // Send a trace to all delilahs
        void sendTrace( std::string type , std::string context , std::string message );

        // Get a collection with a single record with information for this worker...
        network::Collection* getWorkerCollection( Visualization* visualization );

        
        
	};
	
}

#endif
