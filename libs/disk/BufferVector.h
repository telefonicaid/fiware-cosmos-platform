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
		size_t size;					// size of the file it will generate ( both txt and kvs )
		
		network::Queue* queue;			// Information about the queue we are buffering buffers to
		bool txt;						// Flag to indicate if incomming buffers are txt buffers
		
		size_t task_id;					// Task id associated with this new file

		// Constructor
		QueueuBufferVector(size_t task_id , const network::Queue & _queue , bool _txt );
		
		//Destructor
		~QueueuBufferVector();
		
		// Function to add a buffer to the vector
		void addBuffer( Buffer *b );
		
		// Clear the buffer ( after producing a new joined file)
		void clear();

		// Get joined buffer from the accumulated buffers ( both txt and kvs )
		Buffer* getJoinedBufferAndClear();
		
		
	private:
		
		Buffer* getFileBufferFromNetworkBuffers( );
		Buffer* getTXTBufferFromBuffers();		
		
	};
	
	
}

#endif
