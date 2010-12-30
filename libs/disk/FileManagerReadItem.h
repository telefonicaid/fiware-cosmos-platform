
#ifndef _H_FILE_MANAGER_READ_ITEM
#define _H_FILE_MANAGER_READ_ITEM


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
	
	
	class FileManagerReadItem : public FileManagerItem
	{
		
	public:
		
		std::string fileName;				// Name of the file
		size_t offset;						// Offset inside file ( only used when reading )
		size_t size;						// Size to read
		
		char *buffer;						// Pointer to where data should be written
				
		FileManagerReadItem( std::string _fileName , size_t _offset , size_t _size ,  char *_buffer , FileManagerDelegate *_delegate );
		
		// Replace the delegate ( before sending to File Manager )
		void setDelegate( FileManagerDelegate *_delegate );
		
		void getStatus( std::ostream &output , std::string prefix_per_line );
		
	};	
}


#endif
