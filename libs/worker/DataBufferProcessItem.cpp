
#include "DataBufferProcessItem.h"		// Own interface
#include "Buffer.h"						// ss::Buffer
#include "MemoryManager.h"				// ss::MemoryManager
#include "DiskManager.h"				// ss::DiskManager
#include "FileManager.h"				// ss::FileManager	
#include "BufferVector.h"				// ss::BufferVector
#include "DataBufferItemDelegate.h"		// ss::DataBufferItemDelegate
#include "DataBuffer.h"					// ss::DataBuffer
#include "SamsonWorker.h"				// ss::SamsonWorker
#include "Packet.h"						// ss::Packet
#include "SamsonSetup.h"				// ss::SamsonSetup
#include <cstring>						// size_t
#include <string>						// std::string
#include "au_map.h"						// au::map
#include "Lock.h"						// au::Lock
#include <vector>						// std::vector
#include "coding.h"						// ss::hg_info , ss::hg_size 
#include "Buffer.h"						// ss::Buffer
#include "MemoryManager.h"				// ss::MemoryManager
#include "DiskManagerDelegate.h"		// ss::DiskManagerDelegate
#include <set>							// std::set
#include "samson.pb.h"					// ss::network::...
#include "BufferVector.h"				// ss::BufferVector
#include "ProcessItem.h"				// ss::ProcessItem
#include "FileManagerWriteItem.h"		// ss::FileManagerWriteItem

namespace ss {
	
	DataBufferProcessItem::DataBufferProcessItem( QueueuBufferVector * _bv ) : ProcessItem( ProcessItem::pure_process )
	{
		bv = _bv;
		
		// Set the status 
		std::ostringstream statusStream;
		statusStream << "Prewrite of " << au::Format::string( bv->size ,"Bytes" ) ;
		setStatus( statusStream.str() );
	}
	
	void DataBufferProcessItem::run()
	{
		// Joint the buffer to get a new one
		buffer =  bv->getJoinedBufferAndClear();
	}	
}
