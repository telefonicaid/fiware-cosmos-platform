

#include "engine/MemoryRequest.h"		// Own interface
#include "engine/Buffer.h"				// samson::Buffer
#include "logMsg/logMsg.h"				// LM_X

namespace engine
{

	
    MemoryRequest::MemoryRequest( au::Environment *_environment )
    {
        if( !_environment->isSet("size") )
            LM_X(1,("MemoryRequest without size parameter"));
        
        copyFrom( _environment );
        size = getSizeT("size", 0);
    }

	
}