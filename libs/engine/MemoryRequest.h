#ifndef _H_MEMORY_REQUEST
#define _H_MEMORY_REQUEST


#include <cstring>
#include "engine/Object.h"      // engine::Object


namespace engine {

    /**
     Information about a particular request
     */

    class Buffer;
    
    class MemoryRequest : public Object
    {
        
    public:
        
        // Size of the memory requets ( extracted from environment )
        size_t size;
        
        // Listner to notify
        size_t listner_id;

        // Buffer alocated for this
        Buffer *buffer;

        // Basic constructor
        MemoryRequest( size_t _size , size_t _listener_id );
        
    };    
    
	
}

#endif
