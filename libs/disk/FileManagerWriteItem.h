
#ifndef _H_FILE_MANAGER_WRITE_ITEM
#define _H_FILE_MANAGER_WRITE_ITEM


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
#include "FileManagerItem.h"		// ss::FileManagerItem


namespace ss {
	
	class FileManagerWriteItem : public FileManagerItem
	{
		
	public:
		
		std::string fileName;			// FileName of the file
		Buffer *buffer;					// Buffer with onMemory information
		
		
		FileManagerWriteItem( std::string _fileName , Buffer *_buffer , FileManagerDelegate *_delegate ) ;
		
		void getStatus( std::ostream &output , std::string prefix_per_line );
		
	};
	
}


#endif
