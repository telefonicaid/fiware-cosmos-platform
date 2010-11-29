
#ifndef _H_ENVIRONMENT_OPERATIONS
#define _H_ENVIRONMENT_OPERATIONS


#include "samson/Environment.h"			// ss::Environment
#include "samson.pb.h"					// ss::network::...

namespace ss {

	void copyEnviroment( Environment* from , network::Environment * to );
	
	void copyEnviroment( const network::Environment & from , Environment* to  );

}

#endif