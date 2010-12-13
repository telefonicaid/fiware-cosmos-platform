
#ifndef _H_FILE_MANAGER_CACHE_SYSTEM
#define _H_FILE_MANAGER_CACHE_SYSTEM


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

	
	class FileManagerCacheItem;
	class FileManagerReadItem;
	class FileManagerWriteItem;
	class FileManagerCacheTask;
	
	void* runFileManagerCacheSystemThread(void*);
	
	class FileManagerCacheSystem
	{
		
		// Cache of files
		size_t size_in_cache;										// Total memory used by cache
		int num_files_in_cache;												// Number of files in cache
		au::ListMap <std::string , FileManagerCacheItem> cache;		// Double-linked list of cache elements
		
		// List of pending tasks
		std::list<FileManagerCacheTask*> tasks;
		
		pthread_t t;	// Main thread of file manager cache system
		
		au::Lock lock;
		au::StopLock stopLock;
		
	public:
		
		FileManagerCacheSystem();
		
		void run();
		
		void freeMemoryIfPossible( double memory_per);
		
		/**
		 Add a request for a file
		 Return true, if it is in the cache and can be served
		 */
		
		bool addReadItem( FileManagerReadItem *read_item );
		
		/** 
		 Add a buffer to the cache
		 */
		
		void addWriteItem( FileManagerWriteItem *write_item );
		
		/**
		 Notify that the write item is on disk
		 */
		
		void notifyWriteItemFinished(  FileManagerWriteItem *write_item );
		
		
		std::string getStatus();
		
	};
	
	
}


#endif
