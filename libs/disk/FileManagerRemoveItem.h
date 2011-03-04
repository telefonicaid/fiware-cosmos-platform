

#ifndef _H_FILE_MANAGER_REMOVE_ITEM
#define _H_FILE_MANAGER_REMOVE_ITEM


#include "Lock.h"					// au::Locod
#include "Buffer.h"					// ss::Buffer
#include "au_map.h"					// au::map
#include <list>						// std::list
#include <set>						// std::set
#include <vector>					// std::vector
#include "MemoryManager.h"			// ss::MemoryManager
#include "DiskManager.h"			// ss::DiskManager
#include "DiskManagerDelegate.h"	// ss::DiskManagerDelegate
#include "DiskStatistics.h"			// ss::DiskStatistics
#include "FileManagerItem.h"		// ss::FileManagerItem


namespace ss {
	
	
	/**
	 Request of a file to be removed
	 */
	
	class FileManagerRemoveItem : public FileManagerItem
	{
		
	public:
		
		std::string fileName;				// Name of the file
		
		FileManagerRemoveItem( std::string _fileName , FileManagerDelegate* delegate ) : FileManagerItem( delegate ,  FileManagerItem::remove  , 0 )
		{
			fileName = _fileName;
		}
		
		void notifyToDelegate()
		{
			if( delegate )
				delegate->notifyFinishRemoveItem(this);
			else
				delete this;	// Auto-remove to myself since no-one will handle me
		}
		
		void freeResources()
		{
			// nothing to do here
		}
		
		
	};
	
	
}


#endif


