
#ifndef _H_BufferSinkInterface
#define _H_BufferSinkInterface

#include "Buffer.h"			// ss::Buffer

namespace ss
{
	class BufferSinkInterface
	{
	 public:
		virtual void newBuffer( Buffer * buffer )=0;
		virtual ~BufferSinkInterface() {}
	};
}


#endif
