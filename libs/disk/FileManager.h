
/* ****************************************************************************
 *
 * FILE                     FileManager.h
 *
 * AUTHOR                   Andreu Urruela
 *
 * CREATION DATE            2010
 *
 */

/*
 
 FileManager is the singleton class that organizes the operations to disk.
 It uses the underlying DiskManager to schedule tasks like read, write etc.. if necessary
 At this level, we include a cache system to avoid exesive read/ write operations and the bridge to a consolidated external storage system
 
 */

#ifndef _H_FILE_MANAGER
#define _H_FILE_MANAGER


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
#include "FileManagerCacheSystem.h"	// ss::FileManagerCacheSystem
#include "samson.pb.h"				// ss::network...

namespace ss {

	
	class FileManagerItem;
	class FileManagerReadItem;
	class FileManagerWriteItem;
	class FileManagerRemoveItem;
	
	/**
	 Class used as interface between samson and DiskManager
	 It is used to send files to be saved on disk and to read part of files
	 */
	 
	class FileManager : public DiskManagerDelegate 
	{
		// thread safe lock
		au::Lock lock;			

		// Counter to give new file manager ids
		size_t current_fm_id;	
		
		// Elements to save or read
		au::map<size_t, FileManagerItem> items;
		
		// Connections between DiskManager ids and FileManager ids
		au::simple_map<size_t , size_t> ids;

		// Cache system for file manager
		// Cache has been disable for low performance ( better implementation is required, multithread in Disk Manager has been removed to assure order in the write - read sequence )
		// FileManagerCacheSystem cacheSystem;
		
		//Statistics
		DiskStatistics statistics;
		
		FileManager()
		{
			// Init the counter to give new ids
			current_fm_id = 0;
		}
				
	public:
		
		/** 
		 Singleton implementation
		 */
		
		static FileManager *shared();
		

		/**
		 Add elements to the File Manager
		 */
		
		size_t addItemToRead( FileManagerReadItem* v );
		size_t addItemToWrite( FileManagerWriteItem* v );
		size_t addItemToRemove( FileManagerRemoveItem* v );
		
		/**
		 Delegate interface of DiskManager
		 It is notified when a write / read is finished
		 */
		
		void diskManagerNotifyFinish( size_t id, bool error , std::string error_message);
		
		void finishItem( size_t fm_id , bool error , std::string error_message );		
		
		void fill(network::WorkerStatus*  ws);
		
		
	};


}
#endif