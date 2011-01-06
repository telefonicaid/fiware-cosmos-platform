#ifndef _H_DATA_BUFFER
#define _H_DATA_BUFFER

#include <cstring>			// size_t
#include <string>			// std::string
#include "au_map.h"			// au::map
#include "Lock.h"			// au::Lock
#include <vector>			// std::vector
#include "coding.h"			// ss::hg_info , ss::hg_size 
#include "Buffer.h"			// ss::Buffer
#include "MemoryManager.h"	// ss::MemoryManager
#include "DiskManagerDelegate.h"	// ss::DiskManagerDelegate && ss::FileManagerDelegate
#include <set>						// std::set
#include "Status.h"		// getStatusFromArray(.)
#include "samson.pb.h"				// network::...
#include "au_map.h"					// au::simple_map
#include "Status.h"				// au::Status

namespace ss {

	// Static function to work with Buffers
	
	
	class Buffer;
	class DataBufferItem;
	class DataBufferItemDelegate;
	class SamsonWorker;
	class QueueuBufferVector;
	
	/**
	 This class acumulate data buffers coming from network interface
	 When enougth data is accumulated it is frozen to a file and DataManager if notified
	 */
	
	class DataBuffer : public FileManagerDelegate 
	{
		
		au::map<size_t , DataBufferItem> item;
		
		au::Lock lock;				// mutex to protect multiple thread access
		au::StopLock stopLock;		// Stop lock for the background thread
		
		au::simple_map<size_t,size_t> id_relation;	// Relation between diskManager ids and task ids
		
		// List of Vector Buffers pending to be saved to disk ( they are processed by a parallel thread to create files )
		std::list<QueueuBufferVector*> pendingBufferVectors;

		friend class DataBufferItem; // to acces pendingBuffersVectors while protected with the local lock
		
	public:

		SamsonWorker *worker;
				
		DataBuffer( SamsonWorker *_worker );

		// Main routine to the background thread
		void runBackgroundThreadToProcessBufferVectors();
		
		// Add a buffer to the system ( it goes for a particular task_id and a queue )
		void addBuffer( size_t task_id , network::Queue , Buffer* buffer , bool txt  );
		
		// Notification that a worker has send "close data exhcange" for this task
		void finishWorker( size_t task_id );
				
		// FileManagerDelegate
		void fileManagerNotifyFinish(size_t id, bool success);

		// Fill information in the message
		void fill(network::WorkerStatus*  ws);
		
	private:
		
		// Function to get new file names
		static std::string _newFileName( std::string queue );
		
	};
}

#endif
