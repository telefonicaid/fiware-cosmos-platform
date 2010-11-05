#ifndef _H_DATA_BUFFER_ITEM
#define _H_DATA_BUFFER_ITEM

#include <cstring>			// size_t
#include <string>			// std::string
#include "au_map.h"			// au::map
#include "Lock.h"			// au::Lock
#include <vector>			// std::vector
#include "coding.h"			// ss::hg_info , ss::hg_size 
#include "Buffer.h"			// ss::Buffer
#include "MemoryManager.h"	// ss::MemoryManager
#include "DiskManagerDelegate.h"	// ss::DiskManagerDelegate
#include <set>						// std::set

namespace ss {
	
	class BufferVector;
	class DataBufferItemDelegate;
	
	/**
	 Class to group all the vector of buffers for each output queue
	 */
	
	class DataBufferItem : public au::map<std::string , BufferVector > , public DiskManagerDelegate
	{
		DataBufferItemDelegate *delegate;	// Delegate to sent notifications
		size_t task_id;						// Identifier of the task
		bool finished;						// Flag to indicate that eerything has finished ( necessary to sent the finisghDataBufferTask to delegate )
		
		std::set<size_t> ids_files;			// Collection of ids of "save file" operations pendigng to be confirmed
		au::Lock lock;						// Lock to protect ids_files
		
	public:
		
		DataBufferItem( size_t _task_id , DataBufferItemDelegate * _delegate , std::vector<std::string> queues );
		
		void addBuffer( std::string queue , Buffer *buffer );
		void finish();
		
		void saveBufferToDisk( Buffer* b , std::string filename , std::string queue);
		
		// DiskManagerDelegate
		void diskManagerNotifyFinish(size_t id, bool success);

	private:
		
		/**
		 Internal routine to get the name of a new file
		 */
		
		std::string newFileName( std::string queue);
		
	};
	
}

#endif