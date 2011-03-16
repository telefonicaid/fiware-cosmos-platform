#ifndef SAMSON_MONITOR_H
#define SAMSON_MONITOR_H

#include <assert.h>
#include <fstream>
#include <sstream>
#include <string>
#include <map>
#include <set>
#include <iostream>
#include "MonitorParameter.h"			// ss::MonitorParameter
#include "Lock.h"						// au::Lock
#include "au_map.h"						// au::map

namespace ss {

	class SamsonController;
	class Queue;
	
	
	
	class Monitor
	{
		au::Lock lock;
		
		SamsonController *controller;				// Pointer to samsonController to access controller & data manager
		
		MonitorBlock system;						// Parameter for the system
		au::map<std::string,MonitorBlock> queues;			// Monotor parameter for each Queue
		
		pthread_t t;								// Thread to perform the snapshots
		
	public:
		
		Monitor( SamsonController *controller );
		std::string getJSONString( std::string command );

	public:
		
		void runInBackground();	// Function to start the thread of monitoring
		void run();				// Function called by thread ( not call directly )

		
	};
}

#endif
