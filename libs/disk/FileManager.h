
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
#include "Status.h"					// au::Status
#include "FileManagerCacheSystem.h"	// ss::FileManagerCacheSystem
#include "samson.pb.h"				// ss::network...

namespace ss {

	
	class FileManagerItem;
	class FileManagerReadItem;
	class FileManagerWriteItem;
	
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
		FileManagerCacheSystem cacheSystem;
		
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
		
		
		/**
		 Delegate interface of DiskManager
		 It is notified when a write / read is finished
		 */
		
		void diskManagerNotifyFinish( size_t id, bool success);

		
		void finishItem( size_t fm_id , bool success );		
		
		void fill(network::WorkerStatus*  ws);
		
		
		
	};


}
#endif