#ifndef _H_BUFFER_VECTOR
#define _H_BUFFER_VECTOR

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

	/** 
	 Vector of buffers for a particular queue 
	 */
	
	class BufferVector
	{
		// Total info about the number of kvs and size to write in a file
		FileKVInfo info;	
		
		//Vector of buffers
		std::vector<Buffer*> buffer;
		
	public:
		
		BufferVector();
		
		void addBuffer( Buffer *b );
		
		size_t getSize()
		{
			return info.size;
		}

		FileKVInfo getInfo()
		{
			return info;
		}

		
		Buffer* getFileBufferFromNetworkBuffers( KVFormat queue_format );
		
	};
	
	
}

#endif
