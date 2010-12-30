#include "logMsg.h"             // LM_*

#include "MemoryManager.h"		// Own interface
#include "Buffer.h"				// ss::Buffer
#include "SamsonSetup.h"		// ss:SamsonSetup



namespace ss
{
	static MemoryManager *_memoryManager = NULL;

	MemoryManager::MemoryManager()
	{
		used_memory = 0;
		
		num_buffers = 0;
		
		// Load setup parameters
		num_processes = SamsonSetup::shared()->num_processes;
		
		memory = SamsonSetup::shared()->memory;
		
		shared_memory_size_per_buffer = SamsonSetup::shared()->shared_memory_size_per_buffer;
		shared_memory_num_buffers = SamsonSetup::shared()->shared_memory_num_buffers;
		

		assert( shared_memory_size_per_buffer > 0);
		
		// Boolean vector showing if a buffer is used
		shared_memory_used_buffers = (bool*) malloc( shared_memory_num_buffers * sizeof(bool ) );
		for (int i = 0 ; i < shared_memory_num_buffers ; i++)
			shared_memory_used_buffers[i] = false;
		
		
		//Setup run-time status
		setStatusTile( "Memory manager" , "mm" );
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
		for (int i = 0 ; i < num_processes ;i++)
			createSharedMemory(i);
	}
	
	SharedMemoryItem* MemoryManager::getSharedMemory( int i )
	{
		assert( i >= 0);
		
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
		size = shared_memory_size_per_buffer;
		shmflg = IPC_CREAT | 384;			// Permission to read / write ( only owner )
		
		if ((_info->shmid = shmget (key, size, shmflg)) == -1)
		{
			perror("shmget: shmget failed"); 
			std::cerr << "Error with shared memory when creating shared memory\n";
			exit(1); 
		}
		
		// Attach to local-space memory
		_info->data = (char *)shmat(_info->shmid, 0, 0);
		if( _info->data == (char*)-1 )
		{
			perror("shmat: shmat failed"); 
			std::cerr << "Error with shared memory while attaching to local memory\n";
			exit(1);
		}
		_info->size = size;
		
		items.insert( std::pair<int,SharedMemoryItem*>( i , _info) );
		
		lock.unlock();
		
		return _info;
	}		
	
	Buffer *MemoryManager::newBuffer( std::string name , size_t size )
	{
		lock.lock();
		
		// Keep counter of the used memory
		used_memory += size;
		
		
		// Increase the number of used memory buffers
		num_buffers++;
		
		Buffer *b = new Buffer( name, size );

		lock.unlock();
		return b;
	}
	
	void MemoryManager::destroyBuffer(Buffer* b)
	{
		if (b == NULL)
			return;

		lock.lock();
		
		// Keep counter of the used memory
		used_memory -= b->getMaxSize();
		
		//LM_T( 0 , ("destroy buffer with max size %sbytes", au::Format::string( b->getMaxSize() ).c_str() ) );
		
		// Decrease the number of used buffers
		num_buffers--;
		
		b->free();
		delete b;
		
		lock.unlock();
		

	}
	

	
	double MemoryManager::getMemoryUsage()
	{
		double per;
		if( memory == 0 )
			per = 0;
		else
			per =  ( (double) used_memory / (double)memory );

		return per;
	}

	void MemoryManager::getStatus( std::ostream &output , std::string prefix_per_line )
	{
		int num_shm_buffers = 0;
		for (int i = 0 ; i < shared_memory_num_buffers ; i++)
			if( shared_memory_used_buffers[i] )
				num_shm_buffers++;
		
		int per_memory = (int) ( getMemoryUsage()*100.0 );
		output <<"\n";	// All in new lines
		output << prefix_per_line << "Used memory: " << au::Format::string( used_memory ) << " / " << au::Format::string(memory) << " (" << per_memory << "%)"<< std::endl;
		output << prefix_per_line << "Buffers in action " << num_buffers << std::endl;
		output <<  prefix_per_line << "Shared memory Buffers " << num_shm_buffers << " / " << shared_memory_num_buffers << std::endl;
		output <<  prefix_per_line << "Buffers: ";
		std::set<Buffer*>::iterator iter;
		for (iter = buffers.begin() ; iter != buffers.end() ; iter++ )
			output << (*iter)->_name << " ";
		output << std::endl;
		
	}	
	
	// Fill information
	void MemoryManager::fill(network::WorkerStatus*  ws)
	{
		
		int num_shm_buffers = 0;
		for (int i = 0 ; i < shared_memory_num_buffers ; i++)
			if( shared_memory_used_buffers[i] )
				num_shm_buffers++;
		
		std::ostringstream output;

		int per_memory = (int) ( getMemoryUsage()*100.0 );
		output << "Used: " << au::Format::string( used_memory ) << " / " << au::Format::string(memory) << " (" << per_memory << "%)";
		output << " Buffers in action: " << num_buffers;
		output << " Shared memory Buffers: " << num_shm_buffers << " / " << shared_memory_num_buffers;
		ws->set_memory_status( output.str() );
		
		ws->set_used_memory( used_memory );
		ws->set_total_memory( memory );
	}
	

}
