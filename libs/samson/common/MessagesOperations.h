
#ifndef _H_MESSAGES_OPERATIONS
#define _H_MESSAGES_OPERATIONS

#include "samson/common/samson.pb.h"	// network:...
#include "samson/module/KVFormat.h"     // KVFormat
#include "samson/common/samson.pb.h"    // network::

namespace samson {

	bool filterName( const std::string& name , const std::string& begin , const std::string& end);
	
}

#endif