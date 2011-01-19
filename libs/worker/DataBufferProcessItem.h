#ifndef _H_DATA_BUFFER_ITEM
#define _H_DATA_BUFFER_ITEM


#include "ProcessItem.h"		// ss::ProcessItem

namespace ss {
	
	class QueueuBufferVector;
	class Buffer;
	
	
	class DataBufferProcessItem : public ProcessItem
	{
		
	public:
		
		QueueuBufferVector *bv;		// Input vector of buffers
		Buffer *buffer;				// Resulting new buffer
		
		DataBufferProcessItem( QueueuBufferVector * _bv );
		
		void run();
	};
	

	
}

#endif