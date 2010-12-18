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
#include "Status.h"				// au::Status

namespace ss {
	
	class DataBufferItemDelegate;
	class DatBuffer;
	
	
	
	/**
	 Class to group all the vector of buffers for each output queue
	 */
	
	class DataBufferItem : public au::map<std::string , QueueuBufferVector> , public au::Status
	{
		DataBuffer *dataBuffer;				// Pointer to the buffer
		
		size_t task_id;						// Identifier of the task

		int pending_files_to_be_created;	// Number of files that need to ne created( in a differente thread )
		int pending_files_to_be_saved;		// Number of files pending to be confirmed by File Manager
		
		// Information about workers
		int num_workers;					// Total number of workers
		int num_finished_workers;			// Number of workers that have notified they are finished

		bool finish_task_notified;			// Flat to indicate if we already notified about the finish of this task to the TaskController
		
		au::Lock lock;						// Lock to proytect 
		
	public:
		
		bool to_be_removed;					// Flag used by Data Buffer to remove this item

		
		DataBufferItem( DataBuffer *buffer, size_t _task_id ,  int num_workers  );
		
		// Add a buffer to this item ( return if new Buffer vectors have been added)
		bool addBuffer( network::Queue queue , Buffer *buffer , bool txt ); 
		
		// Notification that the background thread has created a new file
		void newFileCreated();
		
		// Notification that a worker has finished sending us data ( return if new Buffer vectors have been added)
		bool finishWorker();

		// Unified function to check the finish, complete of the task
		void check();
		
		// Function to get the run-time status of this object
		void getStatus( std::ostream &output , std::string prefix_per_line );
		
		// Get information about this item
		std::string getStatus();
		
	private:

		void saveBufferToDisk( Buffer* b , std::string filename ,network::Queue *queue ,  bool txt );

		
		/**
		 Internal routine to get the name of a new file
		 */
		
		
		friend class DataBuffer;
		
		// Notification (Trougth Data Buffer) that a file has been saved to disk
		void fileManagerNotifyFinish(size_t id, bool success);
		
	};
	
}

#endif