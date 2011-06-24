

#include "logMsg/logMsg.h"				// LM_X

#include "engine/MemoryRequest.h"		// Own interface
#include "engine/Buffer.h"				// samson::Buffer

namespace engine
{

	
    MemoryRequest::MemoryRequest( size_t _size , size_t _listener_id )
    {
        size = _size;
        listner_id = _listener_id;
        
        buffer = NULL;
    }

	
}