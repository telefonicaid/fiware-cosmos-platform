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

namespace ss {

	// Static function to work with Buffers
	std::string BufferToString( Buffer *b );
	hg_info BufferGetTotalInfo( Buffer *b );
	hg_info BufferGetInfo( Buffer *b , ss_hg hg );
	
	class Buffer;
	class DataBufferItem;
	class DataBufferItemDelegate;
	
	/**
	 This class acumulate data buffers coming from network interface
	 When enougth data is accumulated it is frozen to a file and DataManager if notified
	 */
	
	class DataBuffer : public au::map<size_t , DataBufferItem>
	{
		
		au::Lock lock;	// mutex to protect multiple thread access

	public:
		
		/**
		 Inform that a partiruclar task ( id ) will send key-values to a particular queue
		 */
				
		void newTask( size_t task_id , DataBufferItemDelegate* delegate , std::vector<std::string> queues );
		
		/**
		 New packet from the network interface
		 */
		
		void addBuffer( size_t task_id , std::string queue , Buffer* buffer );
		
		/**
		 Inform that a particular task is finished.. so no more addBuffer calls
		 After this call, the delegate should receive a call to finishDataBufferTask soon.
		 */
		
		void finishTask( size_t task_id );
	
		
		/**
		 Remove task in the data buffer
		 */
		
		void removeTask( size_t task_id );
		
		
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
		
	};
}

#endif
