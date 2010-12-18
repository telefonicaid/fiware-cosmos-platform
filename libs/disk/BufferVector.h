#ifndef _H_BUFFER_VECTOR
#define _H_BUFFER_VECTOR

#include <cstring>			// size_t
#include <string>			// std::string
#include "au_map.h"			// au::map
#include "Lock.h"			// au::Lock
#include <vector>			// std::vector
#include "coding.h"			// ss::hg_info , ss::hg_size 
#include "Buffer.h"					// ss::Buffer
#include "MemoryManager.h"			// ss::MemoryManager
#include "DiskManagerDelegate.h"	// ss::DiskManagerDelegate
#include <set>						// std::set

namespace ss {

	/** 
	 Vector of buffers for a particular queue 
	 */
	
	class QueueuBufferVector	
	{

	public:
		
		std::vector<Buffer*> buffer;	// Vector of buffers
		FileKVInfo info;				// Total info about the number of kvs and size to write in a file 

		size_t size;	// size of the file it will generate
		
		network::Queue queue;
		bool txt;
		
		QueueuBufferVector( network::Queue _queue , bool _txt );
		
		void addBuffer( Buffer *b );
		
		void init();
		
		FileKVInfo getInfo()
		{
			return info;
		}
		
		size_t getSize()
		{
			return size;
		}
		
		// Get a global buffer from previous ones
		
		Buffer* getFileBufferFromNetworkBuffers( KVFormat queue_format );
		Buffer* getTXTBufferFromBuffers();		
		
	};
	
	
}

#endif
