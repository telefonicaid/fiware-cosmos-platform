#ifndef _H_DATA_MANAGER_ITEM
#define _H_DATA_MANAGER_ITEM

#include <iostream>
#include <fstream>			    // ifstream , ofstream

#include "logMsg/logMsg.h"             // LM_*
#include "logMsg/traceLevels.h"        // Trace Levels

#include "samson/data/data.pb.h"
#include "samson/common/traces.h"
#include "au/Lock.h"				// au::Lock
#include "au/map.h"				// au::map
#include <sstream>				// std::ostringstream

namespace samson {

	class DataManager;
	
	class DataManagerItem
	{
	public:
		
		size_t task_id;                         // Id of the task
		std::vector<std::string> command;       // List of commands for this task
		
		DataManagerItem( size_t id )
		{
			task_id = id;
		}
		
		void addCommand( std::string c )
		{
			command.push_back(c);
		}
		
		void run( DataManager *manager );
		
	};
}

#endif
