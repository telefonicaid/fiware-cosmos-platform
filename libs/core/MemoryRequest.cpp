

#include "MemoryRequest.h"		// Own interface
#include "Buffer.h"				// ss::Buffer
#include "logMsg.h"				// LM_X
#include "EngineDelegates.h"

namespace ss
{
	
	MemoryRequest::MemoryRequest( size_t _size , Buffer **_buffer,  MemoryRequestDelegate *_delegate )
	{
		size = _size;
		buffer = _buffer;
		delegate = _delegate;	// There is no sence a request for memory with no delegate ;)
	}
	
	
}