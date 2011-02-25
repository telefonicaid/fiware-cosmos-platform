
#ifndef _H_MESSAGES_OPERATIONS
#define _H_MESSAGES_OPERATIONS

#include "samson.pb.h"	// network:...

namespace ss {

	bool evalHelpFilter( network::Help *help , std::string name);
	bool filterName( const std::string& name , const std::string& begin , const std::string& end);
	
}

#endif