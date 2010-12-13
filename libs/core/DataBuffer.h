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
	
	/**
	 This class acumulate data buffers coming from network interface
	 When enougth data is accumulated it is frozen to a file and DataManager if notified
	 */
	
	class DataBuffer : public FileManagerDelegate , public au::Status
	{
		
		au::map<size_t , DataBufferItem> item;
		
		au::Lock lock;	// mutex to protect multiple thread access

		friend class DataBufferItem;
		au::simple_map<size_t,size_t> id_relation;	// Relation between diskManager ids and task ids
		
	public:

		SamsonWorker *worker;
				
		DataBuffer( SamsonWorker *_worker );
		
		/**
		 New packet from the network interface
		 */
		
		void addBuffer( size_t task_id , network::Queue , Buffer* buffer , bool txt  );
		
		/**
		 Inform that a particular task has receive a close message form a worker finished.
		 */
		
		void finishWorker( size_t task_id );
		
		// Function to get the run-time status of this object
		void getStatus( std::ostream &output , std::string prefix_per_line );
				
		// FileManagerDelegate
		void fileManagerNotifyFinish(size_t id, bool success);

		// Fill information in the message
		void fill(network::WorkerStatus*  ws);
		
		
	};
}

#endif
