#ifndef _H_DATA_BUFFER_ITEM
#define _H_DATA_BUFFER_ITEM


#include "engine/ProcessItem.h"		// samson::ProcessItem
#include "engine/Buffer.h"

namespace samson {
	
	class QueueuBufferVector;
	
	
	class DataBufferProcessItem : public engine::ProcessItem
	{
		
	public:
		
		QueueuBufferVector *bv;		// Input vector of buffers
		engine::Buffer *buffer;				// Resulting new buffer
		
		DataBufferProcessItem( QueueuBufferVector * _bv );
		~DataBufferProcessItem();
        
		void run();
	};
	

	
}

#endif