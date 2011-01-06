
#ifndef _H_FILE_MANAGER_ITEM
#define _H_FILE_MANAGER_ITEM


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
	 Generic read/write item that contains a pointer to the delegate
	 */
	
	class FileManagerItem 
	{
	protected:
		
		FileManagerDelegate *delegate;		// Delegate that should be notified
		
		size_t fm_id;						// File Manager id
		
		friend class FileManager;
		
		//For statistics
		
		DiskStatistics::Operation operation;
		
		size_t size;
		
		timeval begin;
		timeval end;
		
	public:
		
		
		typedef enum
		{
			write,
			read
		} Type;
		
		Type type;
		
		FileManagerItem( FileManagerDelegate* _delegate ,  Type _type , size_t _size );
		
		size_t getId();
		
		void addStatistics(  DiskStatistics * statistics );
		
	private:
		void setId(size_t _id);
		
		
	};	
}


#endif
