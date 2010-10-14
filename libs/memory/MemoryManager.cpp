
#include "MemoryManager.h"		// Own interface
#include "Buffer.h"				// ss::Buffer

namespace ss
{
	static MemoryManager *_memoryManager = NULL;

	MemoryManager::MemoryManager()
	{
		used_memory = 0;
	}
	
	MemoryManager* MemoryManager::shared()
	{
		if( !_memoryManager )
		   _memoryManager = new MemoryManager ();
		return _memoryManager;
	}
	
	Buffer *MemoryManager::newPrivateBuffer( size_t size )
	{
		lock.lock();
		
		used_memory += size;
		Buffer *tmp = new Buffer( -1 , (char*) malloc(size) , size );
		lock.unlock();
		return tmp;
	}

	
	void MemoryManager::destroy( Buffer *b)
	{
		lock.lock();
		used_memory -= b->getSize();
		
		if( b->_sharedMemoryId == -1)
		{
			b->free();
		}
		else
		{
			// Nothing since it is shared memory
		}
		
		delete b;
		
		lock.unlock();

	}
	
	

}
