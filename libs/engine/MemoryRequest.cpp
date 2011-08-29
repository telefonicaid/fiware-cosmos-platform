

#include "logMsg/logMsg.h"				// LM_X

#include "au/Format.h"          // au::xml_

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

    void MemoryRequest::getInfo( std::ostringstream& output)
    {
        au::xml_open( output , "memory_request" );
        au::xml_close( output , "memory_request" );
    }
	
}