#ifndef _H_MEMORY_REQUEST
#define _H_MEMORY_REQUEST


#include <cstring>
#include "samson/Environment.h"

namespace ss {

    /**
     Information about a particular request
     */
    
    class MemoryRequest : public  Environment
    {
        
    public:
        
        // Size of the memory requets ( extracted from environment )
        size_t size;

        // Basic constructor
        MemoryRequest( Environment *environment );
        
    };    
    
	
}

#endif
