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
#include "samson.pb.h"				// ss::network::...
#include "BufferVector.h"			// ss::BufferVector

namespace ss {
	
	class DataBufferItemDelegate;
	class DatBuffer;
	
	
	/**
	 Each buffer vector for a particular queue
	 */
	
	class QueueuBufferVector : public BufferVector
	{
		
	public:
		network::Queue queue;
		
		QueueuBufferVector( network::Queue _queue )
		{
			queue = _queue;
		}
		
		
	};
	
	/**
	 Class to group all the vector of buffers for each output queue
	 */
	
	class DataBufferItem : public au::map<std::string , QueueuBufferVector  > , public DiskManagerDelegate
	{
		DataBuffer *dataBuffer;					// Pointer to the buffer
		
		size_t task_id;						// Identifier of the task
		
		bool finished;						// Flag to indicate that eerything has finished
		bool completed;						// Flag to indicate that the operation is completed ( finish and saved )
		
		std::set<size_t> ids_files;			// Collection of ids of "save file" operations pendigng to be confirmed
		std::set<size_t> ids_files_saved;	// Collection of ids of "save file" operations pendigng to be confirmed
		au::Lock lock;						// Lock to protect ids_files
		
		int num_finished_workers;			// Number of workers that have notified they are finished
		int num_workers;					// Total number of workers
		
	public:
		
		std::vector<network::QueueFile> qfiles;	// Created files to be notified to the controller
		
		DataBufferItem( DataBuffer *buffer, size_t _task_id , int num_workers );
		
		void addBuffer( network::Queue queue , Buffer *buffer );

		void finishWorker();
		
		void saveBufferToDisk( Buffer* b , std::string filename ,network::Queue );
		
		// DiskManagerDelegate
		void diskManagerNotifyFinish(size_t id, bool success);

		// Get some string with debug info
		std::string getStatus();
			
		
		
	private:
		
		/**
		 Internal routine to get the name of a new file
		 */
		
		std::string newFileName( std::string queue);
		
	};
	
}

#endif