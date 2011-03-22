#ifndef _H_MEMORY_REQUEST
#define _H_MEMORY_REQUEST


#include <cstring>

namespace ss {
	
	class MemoryRequest;
	class Buffer;
	class MemoryRequestDelegate;
	
	/**
	 Object used to request memory for a new operation
	 */
	
	class MemoryRequest
	{
		
	public:
		
		size_t size;// Required size
		Buffer **buffer;// Provides buffer
		MemoryRequestDelegate *delegate;// Delegate to notify when ready
		
		// Reference elements ( to be used in the delegate notification to identify this memory request )
		int component;
		size_t tag;
		size_t sub_tag;
		
		MemoryRequest( size_t _size , Buffer **_buffer, MemoryRequestDelegate *_delegate );
		
	};
	
}

#endif
