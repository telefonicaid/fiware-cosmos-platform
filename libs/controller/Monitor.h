#ifndef SAMSON_MONITOR_H
#define SAMSON_MONITOR_H

#include <fstream>
#include <sstream>
#include <string>
#include <map>
#include <set>
#include <iostream>
#include "MonitorParameter.h"			// ss::MonitorParameter
#include "au/Lock.h"						// au::Lock
#include "au/map.h"						// au::map

namespace ss {

	class SamsonController;
	class Queue;
	
	class Monitor
	{
		
		SamsonController *controller;				// Pointer to samsonController to access controller & data manager
		MonitorBlock system;						// Parameter for the system
		au::map<std::string,MonitorBlock> queues;	// Monotor parameter for each Queue
		
	public:
		
		Monitor( SamsonController *controller );
        ~Monitor();
        
		std::string getJSONString( std::string command );

	public:
		
		void takeSamples();		// Function to take samples of the controller

		
	};
}

#endif
