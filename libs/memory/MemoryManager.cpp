
#include "MemoryManager.h"		// Own interface
#include "Buffer.h"				// ss::Buffer
#include "SamsonSetup.h"		// ss:SamsonSetup

namespace ss
{
	static MemoryManager *_memoryManager = NULL;

	MemoryManager::MemoryManager() : stopLock(&lock )
	{
		used_memory = 0;
		
		num_buffers = 0;
		
		// Load setup parameters
		num_cores = SamsonSetup::shared()->getInt( SETUP_num_cores , 2);
		shared_memory_per_core = SamsonSetup::shared()->getUInt64( SETUP_shm_size_per_core , 512*1024*1024 );
		memory = SamsonSetup::shared()->getUInt64( SETUP_shm_size_per_core , 0 );
		
	}
	
	MemoryManager* MemoryManager::shared()
	{
		if( !_memoryManager )
		   _memoryManager = new MemoryManager ();
		return _memoryManager;
	}

	// Function to create/load the shared memory items
	void MemoryManager::createSharedMemoryItems()
	{
		// Create shared memory
		for (int i = 0 ; i < num_cores ;i++)
			createSharedMemory(i);
	}
	
	SharedMemoryItem* MemoryManager::getSharedMemory( int i )
	{
		std::map<int,SharedMemoryItem*>::iterator s = items.find( i );	
		if( s == items.end() )
			return createSharedMemory(i);
		else
			return s->second;
	}
	
	SharedMemoryItem* MemoryManager::createSharedMemory( int i )
	{
		lock.lock();
		
		SharedMemoryItem* _info = new SharedMemoryItem();
		
		key_t key;		/* key to be passed to shmget() */ 
		int shmflg;		/* shmflg to be passed to shmget() */ 
		size_t size;	/* size to be passed to shmget() */ 
		
		
		key = SS_SHARED_MEMORY_KEY_ID + i; 
		size = shared_memory_per_core;
		shmflg = IPC_CREAT | 384;			// Permission to read / write ( only owner )
		
		if ((_info->shmid = shmget (key, size, shmflg)) == -1)
		{
			perror("shmget: shmget failed"); 
			exit(1); 
		}
		
		// Attach to local-space memory
		_info->data = (char *)shmat(_info->shmid, 0, 0);
		if( _info->data == (char*)-1 )
		{
			perror("shmat: shmat failed"); 
			exit(1);
		}
		
		items.insert( std::pair<int,SharedMemoryItem*>( i , _info) );
		
		lock.unlock();
		
		return _info;
	}		
	
	Buffer *MemoryManager::newBuffer( size_t size )
	{
		lock.lock();
		
		// Keep counter of the used memory
		used_memory += size;
		
		
		// Increase the number of used memory buffers
		num_buffers++;
		
		Buffer *b = new Buffer( (char*) malloc(size) , size );

		std::cout << "create buffer " << b->getSize() << std::endl;
		
		lock.unlock();
		return b;
	}
	
	void MemoryManager::destroyBuffer( Buffer *b)
	{
		lock.lock();
		
		// Keep counter of the used memory
		used_memory -= b->getSize();
		
		std::cout << "destroy buffer " << b->getSize() << std::endl;
		
		// Decrease the number of used buffers
		num_buffers--;
		
		b->free();
		delete b;
		
		lock.unlock();
		
		// Wake up any thread bloqued for memory usage
		lock.wakeUpStopLock( &stopLock );

	}
	
	Buffer *MemoryManager::newBufferIfMemoryBellow( size_t size , double max_usage_memory , bool blocking )
	{
		while( true )
		{
			Buffer *tmp = NULL;
			lock.lock();
			if( getMemoryUsage() < max_usage_memory )
			{
				used_memory += size;				// Keep counter of the used memory
				tmp = new Buffer( (char*) malloc(size) , size );
				lock.unlock();
				return tmp;
			}
			else
			{
				if( blocking )
					lock.unlock_waiting_in_stopLock( &stopLock );
				else
				{
					lock.unlock();
					return NULL;
				}
			}
		}
	}
	
	double MemoryManager::getMemoryUsage()
	{
		if( memory==0 )
			return 0;
		return (double) used_memory / (double)memory;
	}

	

}
