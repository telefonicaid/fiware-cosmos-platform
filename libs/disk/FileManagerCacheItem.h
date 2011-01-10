
#ifndef _H_FILE_MANAGER_CACHE_ITEM
#define _H_FILE_MANAGER_CACHE_ITEM


#include "Lock.h"		// au::Locod
#include "Buffer.h"		// ss::Buffer
#include "au_map.h"		// au::map
#include <list>			// std::list
#include <set>			// std::set
#include <vector>		// std::vector
#include "MemoryManager.h"			// ss::MemoryManager
#include "DiskManager.h"			// ss::DiskManager
#include "DiskManagerDelegate.h"	// ss::DiskManagerDelegate
#include "DiskStatistics.h"			// ss::DiskStatistics
#include "Status.h"					// au::Status

namespace ss {
	
	/** 
	 Element inside the cache system
	 */
	
	class FileManagerCacheItem
	{
		
	public:
		
		std::string fileName;
		Buffer *buffer;
		
		int used_counter;				// Number of internal operations that lock this element 
		
		// It can be removed from memory is used_counter == 0
		
		FileManagerCacheItem(std::string _fileName , Buffer *_buffer);
		
		bool isUsed();
		
		void destroy();
		
		void retain();
		
		void release();
		
		std::string getStatus()
		{
			std::ostringstream output;
			output << "UD:" << used_counter << " S:" << au::Format::string( buffer->getSize() );
			return output.str();
		}
		
	};
	
}


#endif
