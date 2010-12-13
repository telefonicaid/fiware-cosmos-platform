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
		DataBuffer *dataBuffer;					// Pointer to the buffer
		
		size_t task_id;						// Identifier of the task
		
		bool finished;						// Flag to indicate that eerything has finished
		bool completed;						// Flag to indicate that the operation is completed ( finish and saved )
		
		std::set<size_t> ids_files;			// Collection of ids of "save file" operations pendigng to be confirmed
		std::set<size_t> ids_files_saved;	// Collection of ids of "save file" operations pendigng to be confirmed
		
		int num_finished_workers;			// Number of workers that have notified they are finished

		int myWorkerId;						// My worker id to notify controller about this
		int num_workers;					// Total number of workers

		
	public:
		
		DataBufferItem( DataBuffer *buffer, size_t _task_id , int myWorkerId ,  int num_workers  );
		
		void addBuffer( network::Queue queue , Buffer *buffer , bool txt );

		void finishWorker();
		
		void saveBufferToDisk( Buffer* b , std::string filename ,network::Queue ,  bool txt );

		// Function to get the run-time status of this object
		void getStatus( std::ostream &output , std::string prefix_per_line );
		
		bool isCompleted()
		{
			return completed;
		}
		
	private:

		/**
		 Internal routine to get the name of a new file
		 */
		
		std::string newFileName( std::string queue);
		
		
		friend class DataBuffer;
		void fileManagerNotifyFinish(size_t id, bool success);
		
	};
	
}

#endif