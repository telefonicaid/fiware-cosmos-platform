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
#include "DiskManagerDelegate.h"	// ss::DiskManagerDelegate
#include <set>						// std::set
#include "ObjectWithStatus.h"		// getStatusFromArray(.)
#include "samson.pb.h"				// network::...

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
	
	class DataBuffer : public au::map<size_t , DataBufferItem> , public DiskManagerDelegate
	{
		
		au::Lock lock;	// mutex to protect multiple thread access

		SamsonWorker *worker;

		friend class DataBufferItem;
		
		std::map<size_t,size_t> id_relation;	// Relation between diskManager ids and task ids
		
	public:
		
		DataBuffer( SamsonWorker *_worker );
		
		/**
		 New packet from the network interface
		 */
		
		void addBuffer( size_t task_id , network::Queue , Buffer* buffer );
		
		/**
		 Inform that a particular task has receive a close message form a worker finished.
		 */
		
		void finishWorker( size_t task_id );
		
		/**
		 Debug information
		 */
		
		std::string str()
		{
			std::ostringstream o;
			
			o << "DataBuffer" << std::endl;
			o << "============================" << std::endl;

			std::map<size_t , DataBufferItem*>::iterator i;
			for (i = begin() ; i != end() ;i++)
				o << "Task " << i->first << std::endl;

			
			return o.str();
		}
		
		std::string getStatus();
		
		
		// DiskManagerDelegate
		void diskManagerNotifyFinish(size_t id, bool success);

		
	};
}

#endif
