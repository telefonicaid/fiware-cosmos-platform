#ifndef _H_DATA_MANAGER_COMMAND_RESPONSE
#define _H_DATA_MANAGER_COMMAND_RESPONSE

#include <iostream>
#include <fstream>			    // ifstream , ofstream

#include "logMsg.h"             // LM_*
#include "traceLevels.h"        // Trace Levels

#include "data.pb.h"
#include "traces.h"
#include "Lock.h"				// au::Lock
#include "au_map.h"				// au::map
#include <sstream>				// std::ostringstream

namespace ss
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
