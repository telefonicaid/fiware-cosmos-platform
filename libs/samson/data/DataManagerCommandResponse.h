#ifndef _H_DATA_MANAGER_COMMAND_RESPONSE
#define _H_DATA_MANAGER_COMMAND_RESPONSE

#include <iostream>
#include <fstream>			    // ifstream , ofstream

#include "logMsg/logMsg.h"             // LM_*
#include "logMsg/traceLevels.h"        // Trace Levels

#include "samson/data/data.pb.h"
#include "samson/common/traces.h"
#include "au/Lock.h"				// au::Lock
#include "au/map.h"				// au::map
#include <sstream>				// std::ostringstream

namespace samson
{

	class DataManagerCommandResponse
	{
		public:

		bool error;
		std::string output;
		
		DataManagerCommandResponse()
		{
			error = false;
		}

	};

}

#endif
