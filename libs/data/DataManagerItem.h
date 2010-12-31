#ifndef _H_DATA_MANAGER_ITEM
#define _H_DATA_MANAGER_ITEM

#include <iostream>
#include <fstream>			  // ifstream , ofstream

#include "logMsg.h"           // LM_*
#include "dataTracelevels.h"  // LMT_*

#include "data.pb.h"
#include "traces.h"
#include "Lock.h"				// au::Lock
#include "au_map.h"				// au::map
#include <sstream>				// std::ostringstream

namespace ss {

	class DataManager;
	
	class DataManagerItem
	{
	public:
		
		size_t task_id;						// Id of the task
		std::vector<std::string> command;	// List of commands for this task
		
		DataManagerItem( size_t id )
		{
			task_id = id;
		}
		
		void addCommand( std::string c)
		{
			command.push_back(c);
		}
		
		void run( DataManager *manager );
		
	};
}

#endif
