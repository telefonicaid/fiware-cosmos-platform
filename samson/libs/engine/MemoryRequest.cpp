

#include "logMsg/logMsg.h"				// LM_X
#include "logMsg/traceLevels.h"         // LmtMemory

#include "au/xml.h"         // au::xml...


#include "engine/MemoryRequest.h"		// Own interface
#include "engine/Buffer.h"				// samson::Buffer

namespace engine {


MemoryRequest::MemoryRequest( size_t _size , double _mem_percentadge , size_t _listener_id )
{
    size = _size;
    listner_id = _listener_id;
    mem_percentadge = _mem_percentadge;
    LM_T(LmtMemory,("MemoryRequest::MemoryRequest: size:%lu from listender_id:%lu", _size, _listener_id));
    
    if ( size == 0)
        LM_W(("Memory request with size 0"));
    
    buffer = NULL;
}

void MemoryRequest::getInfo( std::ostringstream& output)
{
    au::xml_open( output , "memory_request" );
    au::xml_close( output , "memory_request" );
}

}
