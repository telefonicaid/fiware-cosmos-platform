
#include "DataBufferProcessItem.h"		// Own interface
#include "engine/Buffer.h"						// samson::Buffer
#include "engine/MemoryManager.h"				// samson::MemoryManager
#include "BufferVector.h"				// samson::BufferVector
#include "samson/worker/SamsonWorker.h"				// samson::SamsonWorker
#include "samson/network/Packet.h"						// samson::Packet
#include "samson/common/SamsonSetup.h"				// samson::SamsonSetup
#include <cstring>						// size_t
#include <string>						// std::string
#include "au/map.h"						// au::map
#include "au/Lock.h"						// au::Lock
#include <vector>						// std::vector
#include "samson/common/coding.h"						// samson::hg_info , samson::hg_size 
#include "engine/Buffer.h"						// samson::Buffer
#include "engine/MemoryManager.h"				// samson::MemoryManager
#include <set>							// std::set
#include "samson/common/samson.pb.h"					// samson::network::...
#include "BufferVector.h"				// samson::BufferVector
#include "engine/ProcessItem.h"				// samson::ProcessItem
#include "engine/Buffer.h"

namespace samson {
	
	DataBufferProcessItem::DataBufferProcessItem( QueueuBufferVector * _bv ) : ProcessItem( PI_PRIORITY_BUFFER_PREWRITE )
	{
		bv = _bv;
		
		// Set the status 
		std::ostringstream statusStream;
		statusStream << "Prewrite " << au::str( bv->getTotalSize() ,"Bytes" ) ;
		setProcessItemOperationName( statusStream.str() );
	}
    
    DataBufferProcessItem::~DataBufferProcessItem()
    {
        delete bv;
    }
	
	void DataBufferProcessItem::run()
	{
		// Joint the buffer to get a new one
		buffer =  bv->getJoinedBuffer();
	}	
}
