

#include "MemoryRequest.h"		// Own interface
#include "Buffer.h"				// ss::Buffer
#include "logMsg.h"				// LM_X

namespace ss
{

	
    MemoryRequest::MemoryRequest( Environment *_environment )
    {
        if( !_environment->isSet("size") )
            LM_X(1,("MemoryRequest without size parameter"));
        
        copyFrom( _environment );
        size = getSizeT("size", 0);
    }

	
}