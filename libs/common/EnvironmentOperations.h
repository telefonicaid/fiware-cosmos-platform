
#ifndef _H_ENVIRONMENT_OPERATIONS
#define _H_ENVIRONMENT_OPERATIONS


#include "samson/Environment.h"			// ss::Environment
#include "samson.pb.h"					// ss::network::...
#include "coding.h"                     // KVInfo

namespace ss {

	void copyEnviroment( Environment* from , network::Environment * to );
	
	void copyEnviroment( const network::Environment & from , Environment* to  );
    
    void copy( KVInfo * from , network::KVInfo* to);
    
    void copy( FullKVInfo * from , network::KVInfo* to);
 
    
}

#endif
