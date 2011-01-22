#include "logMsg.h"             // LM_*

#include "MemoryManager.h"		// Own interface
#include "Buffer.h"				// ss::Buffer
#include "SamsonSetup.h"		// ss:SamsonSetup



namespace ss
{
	
	void* runMemoryManagerThread(void*p)
	{
		((MemoryManager*)p)->runThread();
		assert( false );
		return NULL;
	}
	
	static MemoryManager *_memoryManager = NULL;

	void MemoryManager::init()
	{
		assert( !_memoryManager );
		_memoryManager = new MemoryManager ();
		
		// Create the thread to serve memory requests.
		pthread_t t;
		pthread_create(&t, NULL, runMemoryManagerThread, _memoryManager);
		
	}
	
	MemoryManager* MemoryManager::shared()
	{
		assert( _memoryManager );
		return _memoryManager;
	}
	
	
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

		
		// Create the shared memory areas
		for (int i = 0 ; i < shared_memory_num_buffers ; i++)
		{
			
//			LM_M(("Removing shared memory area %d",i));
			removeSharedMemory(i);

//			LM_M(("Creating shared memory area %d",i));			
			SharedMemoryItem *tmp = getSharedMemory(i);
			freeSharedMemory(tmp);
		}
		
	}
	
	
	void MemoryManager::removeSharedMemory( int i )
	{
		key_t key;		/* key to be passed to shmget() */ 
		int shmflg;		/* shmflg to be passed to shmget() */ 
		size_t size;	/* size to be passed to shmget() */ 
		
		key = SS_SHARED_MEMORY_KEY_ID + i; 
		shmflg = IPC_CREAT | 384;			// Permission to read / write ( only owner )
		size = shared_memory_size_per_buffer;
		
		// Get the id
		int id = shmget (key, size, shmflg);

		if( id != -1)
		{
			// Remove
			shmctl( id , IPC_RMID, NULL);
		}
			
	}
	
	SharedMemoryItem* MemoryManager::getSharedMemory( int i )
	{
		SharedMemoryItem* _info = new SharedMemoryItem(i);
		
		key_t key;		/* key to be passed to shmget() */ 
		int shmflg;		/* shmflg to be passed to shmget() */ 
		size_t size;	/* size to be passed to shmget() */ 
		
		
		key = SS_SHARED_MEMORY_KEY_ID + i; 
		size = shared_memory_size_per_buffer;
		shmflg = IPC_CREAT | 384;			// Permission to read / write ( only owner )
		
		if ((_info->shmid = shmget(key, size, shmflg)) == -1)
			LM_RE(NULL, ("shmget: %s", strerror(errno)));
		
		// Attach to local-space memory
		_info->data = (char *)shmat(_info->shmid, 0, 0);
		if( _info->data == (char*)-1 )
		{
			perror("shmat: shmat failed"); 
			std::cerr << "Error with shared memory while attaching to local memory\n";
			assert( false );
			exit(1);
		}
		_info->size = size;
		
		return _info;		
	}
	
	void MemoryManager::freeSharedMemory(SharedMemoryItem* item)
	{
		if (item == NULL)
			LM_RVE(("NULL SharedMemoryItem as input ..."));

		// Detach data
		int ans = shmdt( item->data);
		
		// Make sure operation is correct
		assert( ans != -1);
		
		// remove the item created with getSharedMemory
		delete item;
	}

	
	
	
	Buffer *MemoryManager::newBuffer( std::string name , size_t size )
	{
		token.retain();
		
		// Keep counter of the used memory
		used_memory += size;
		
		
		// Increase the number of used memory buffers
		num_buffers++;
		
		Buffer *b = new Buffer( name, size );

		token.release();
		return b;
	}
	
	void MemoryManager::destroyBuffer(Buffer* b)
	{
		if (b == NULL)
			return;

		token.retain();
		
		// Keep counter of the used memory
		used_memory -= b->getMaxSize();
		
		//LM_T( 0 , ("destroy buffer with max size %sbytes", au::Format::string( b->getMaxSize() ).c_str() ) );
		
		// Decrease the number of used buffers
		num_buffers--;
		
		b->free();
		delete b;
		
		token.release();

		// Wake up background thread
		stopper.wakeUp();

	}
	
	int MemoryManager::retainSharedMemoryArea()
	{
		token.retain();
		
		for (int i = 0  ; i < shared_memory_num_buffers ; i++)
			if ( !shared_memory_used_buffers[i] )
			{
				shared_memory_used_buffers[i] = true;
				token.release();
				
				used_memory += shared_memory_size_per_buffer;
				
				return i;
			}
		
		token.release();
		
		assert(false);
		return -1;	// There are no available memory buffers
	}
	
	
	
	void MemoryManager::releaseSharedMemoryArea( int id )
	{
		assert( id >= 0);
		assert( id < shared_memory_num_buffers);
		
		token.retain();
		shared_memory_used_buffers[id] = false;
		used_memory -= shared_memory_size_per_buffer;
		token.release();
		
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
		

		ws->set_total_memory( memory );
		ws->set_used_memory( used_memory );
	}
	
	
	void MemoryManager::addMemoryRequest( MemoryRequest *request)
	{
		token.retain();
		memoryRequets.push_back( request );
		token.release();
		
		stopper.wakeUp();
		
	}
	
	
	// Function for the main thread of memory
	void MemoryManager::runThread()
	{
		
		while( true )
		{
			double p = getUsedMemory();

			MemoryRequest *r = NULL;
				
			if ( p < 0.5 )
			{
				token.retain();
				r = memoryRequets.extractFront();
				token.release();
			}

			if( !r )
				stopper.stop();	// Stop for no more requets of no memory available
			else
			{
				*(r->buffer) = newBuffer("", r->size);
				r->notifyDelegate();
			}


			
			
		}
		
	}

	

}
