#ifndef _H_MEMORY_REQUEST
#define _H_MEMORY_REQUEST


#include <cstring>
#include "au/Environment.h"

namespace engine {

    /**
     Information about a particular request
     */
    
    class MemoryRequest : public  au::Environment
    {
        
    public:
        
        // Size of the memory requets ( extracted from environment )
        size_t size;

        // Basic constructor
        MemoryRequest( au::Environment *environment );
        
    };    
    
	
}

#endif
