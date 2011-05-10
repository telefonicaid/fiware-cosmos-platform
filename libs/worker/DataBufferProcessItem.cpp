
#include "DataBufferProcessItem.h"		// Own interface
#include "engine/Buffer.h"						// ss::Buffer
#include "engine/MemoryManager.h"				// ss::MemoryManager
#include "BufferVector.h"				// ss::BufferVector
#include "DataBuffer.h"					// ss::DataBuffer
#include "SamsonWorker.h"				// ss::SamsonWorker
#include "Packet.h"						// ss::Packet
#include "SamsonSetup.h"				// ss::SamsonSetup
#include <cstring>						// size_t
#include <string>						// std::string
#include "au/map.h"						// au::map
#include "au/Lock.h"						// au::Lock
#include <vector>						// std::vector
#include "coding.h"						// ss::hg_info , ss::hg_size 
#include "engine/Buffer.h"						// ss::Buffer
#include "engine/MemoryManager.h"				// ss::MemoryManager
#include <set>							// std::set
#include "samson.pb.h"					// ss::network::...
#include "BufferVector.h"				// ss::BufferVector
#include "engine/ProcessItem.h"				// ss::ProcessItem
#include "engine/Buffer.h"

namespace ss {
	
	DataBufferProcessItem::DataBufferProcessItem( QueueuBufferVector * _bv ) : ProcessItem( PI_PRIORITY_BUFFER_PREWRITE )
	{
		bv = _bv;
		
		// Set the status 
		std::ostringstream statusStream;
		statusStream << "Prewrite " << au::Format::string( bv->getTotalSize() ,"Bytes" ) ;
		operation_name = statusStream.str();
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
