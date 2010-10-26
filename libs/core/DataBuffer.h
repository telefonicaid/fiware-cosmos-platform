#ifndef _H_DATA_BUFFER
#define _H_DATA_BUFFER

#include <cstring>		// size_t
#include <string>		// std::string
#include "au_map.h"		// au::map
#include "Lock.h"		// au::Lock
#include <vector>		// std::vector
#include "coding.h"		// ss::hg_info , ss::hg_size 
#include "Buffer.h"		// ss::Buffer

namespace ss {

	
	
	class Buffer;
	

	/** 
	 Vector of buffers for a particular queue 
	 */
	
	class BufferVector : std::vector<Buffer*>
	{
		// Information about the task_id and queue
		size_t task_id;
		std::string queue;
		
		// Total info about the number of kvs and size
		hg_info info;	
		
	public:
		
		BufferVector( size_t _task_id , std::string _queue);
		
		void addBuffer( Buffer *b );
		
		void flushToDisk( );
	};
	
	/**
	 List of vector buffers for each queue
	 */
	
	class TaskDataBuffer : au::map<std::string , BufferVector >
	{
		size_t task_id;
		
	public:
		
		TaskDataBuffer( size_t _task_id )
		{
			task_id = _task_id;
		}
		
		void addBuffer( std::string queue , Buffer *buffer)
		{
			BufferVector* bv = findInMap( queue );
			
			if( !bv )
			{
				bv = new BufferVector( task_id , queue );
				insertInMap( queue	 , bv );
			}

			bv->addBuffer( buffer );
			
		}
		
		void flushToDisk( size_t task_id )
		{
			for ( au::map<std::string , BufferVector >::iterator b = begin() ; b!= end() ; b++)
			{
				BufferVector *bv = b->second;
				bv->flushToDisk( );
				delete bv;
			}
			clear();
		}
		
		
	};
		
	/**
	 This class acumulate data buffers comming from network interface
	 When enougth data is accumulated it is frozen to a file and DataManager if notified
	 */
	
	class DataBuffer : public au::map<size_t , TaskDataBuffer>
	{
		
		au::Lock lock;								// mutex to protect multiple thread access
		
		
		/**
		 New packet from the network interface
		 */
		
		void addBuffer( size_t task_id , std::string queue , Buffer* buffer );
		
		
		/**
		 Close a particular task
		 It basically flush down to disk the last segment into a new file ( incomplete file )
		 */
		
		void closeTask( size_t task_id );
		
		
		
		
		
	};

}

#endif