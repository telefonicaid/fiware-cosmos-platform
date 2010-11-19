#ifndef SAMSON_MONITOR_H
#define SAMSON_MONITOR_H

#include <boost/circular_buffer.hpp>
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
		
		MonitorBlock system;						// Parameter for the system
		std::set<Queue*> queues;					// Queues we are monitoring		
		pthread_t t;								// Thread to perform the snapshots
		
	public:
		
		Monitor( SamsonController *controller);
		std::string getJSONString( std::string command );

	public:
		void run();			// Function called by thread ( not call directly )
		
		void addQueueToMonitor( Queue *queue );
		void removeQueueToMonitor( Queue *queue );
		
	};
}

#endif
