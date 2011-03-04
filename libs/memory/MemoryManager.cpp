/* ****************************************************************************
*
* FILE                     MemoryManager.cpp
*
* AUTHOR                   Andreu Urruela
*
* CREATION DATE            2010
*
*/

#include "logMsg.h"             // LM_*
#include "traceLevels.h"        // Trace Levels

#include "MemoryManager.h"		// Own interface
#include "Buffer.h"				// ss::Buffer
#include "SamsonSetup.h"		// ss:SamsonSetup
#include <sstream>				// std::stringstream
namespace ss
{
	
	MemoryRequest::MemoryRequest( size_t _size , Buffer **_buffer,  MemoryRequestDelegate *_delegate )
	{
		size = _size;
		buffer = _buffer;
		delegate = _delegate;	// There is no sence a request for memory with no delegate ;)
	}
	
	
#pragma mark -----
	
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
		assert( _memoryManager );	// Make sure memory manager has been initialized with init
		return _memoryManager;
	}
	
	
	MemoryManager::MemoryManager()
	{
		// Init usage counters
		used_memory_input=0;
		used_memory_output=0;
		
		num_buffers_input  = 0;
		num_buffers_output = 0;
		
		// Total available memory
		memory = SamsonSetup::shared()->memory;

		// Shared memory setup
		shared_memory_size_per_buffer	= SamsonSetup::shared()->shared_memory_size_per_buffer;
		shared_memory_num_buffers		= SamsonSetup::shared()->num_processes;
		
		assert( shared_memory_size_per_buffer > 0);
		
		// Boolean vector showing if a buffer is used
		shared_memory_used_buffers = (bool*) malloc( shared_memory_num_buffers * sizeof(bool ) );
		for (int i = 0 ; i < shared_memory_num_buffers ; i++)
			shared_memory_used_buffers[i] = false;

		// Create the shared memory areas
		for (int i = 0 ; i < shared_memory_num_buffers ; i++)
		{
			LM_T(LmtMemory, ("Removing shared memory area %d",i));
			removeSharedMemory(i);

			LM_T(LmtMemory, ("Creating shared memory area %d",i));			
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

	
	Buffer *MemoryManager::newBuffer( std::string name , size_t size , Buffer::BufferType type )
	{
		token.retain();

		switch (type) {
			case Buffer::input:
				used_memory_input += size;
				num_buffers_input++;
				break;
			case Buffer::output:
				used_memory_output += size;
				num_buffers_output++;
				break;
		}
		
		Buffer *b = new Buffer( name, size, type );

		token.release();
		return b;
	}
	
	void MemoryManager::destroyBuffer(Buffer* b)
	{
		if (b == NULL)
			return;

		token.retain();
		
		switch (b->getType()) {
			case Buffer::input:
				used_memory_input -= b->getMaxSize();
				num_buffers_input--;
				break;
			case Buffer::output:
				used_memory_output -= b->getMaxSize();
				num_buffers_output--;
				break;
				
		}
		

		LM_T(LmtMemory, ("destroying buffer with max size %sbytes", au::Format::string( b->getMaxSize() ).c_str() ) );
		
		
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
		
		token.release();
		
	}	
	
	// Fill information
	void MemoryManager::fill(network::WorkerStatus*  ws)
	{
		
		int num_shm_buffers = 0;
		for (int i = 0 ; i < shared_memory_num_buffers ; i++)
			if( shared_memory_used_buffers[i] )
				num_shm_buffers++;
		
		std::ostringstream output;

		output << "Input [ Used: " << num_buffers_input << " buffers with " << au::Format::string( used_memory_input , "B" ) << " " << ((int)(getMemoryUsageInput()*100.0)) << "%";
		output << " Output [ Used: " << num_buffers_output << " buffers with " << au::Format::string( used_memory_output , "B" ) << " " << ((int)(getMemoryUsageOutput()*100.0)) << "%";
		ws->set_memory_status( output.str() );

		ws->set_total_memory( memory );
		ws->set_used_memory( getUsedMemory() );
	}
	
	void MemoryManager::addMemoryRequest( MemoryRequest *request)
	{
		if( request->size > memory )
		{
			LM_X(-1,("Error managing memory: excesive memory request"));	
		}

		
		token.retain();
		memoryRequests.push_back( request );
		token.release();
		
		// Wake up the background process to check if 
		stopper.wakeUp();
		
	}
	
	
	// Function for the main thread of memory
	void MemoryManager::runThread()
	{
		
		while( true )
		{
			double p = getMemoryUsageInput();	// Only used for inputs

			MemoryRequest *r = NULL;
				
			if ( p < 0.5 )
			{
				token.retain();
				r = memoryRequests.extractFront();
				token.release();
			}

			if( !r )
				stopper.stop();	// Stop for no more requets of no memory available
			else
			{
				*(r->buffer) = newBuffer("Buffer from request", r->size , Buffer::input);
				r->notifyDelegate();
			}
		}
		
	}
	
	size_t MemoryManager::getUsedMemory()
	{
		return used_memory_input + used_memory_output + shared_memory_num_buffers*shared_memory_size_per_buffer;
	}

	size_t MemoryManager::getUsedMemoryInput()
	{
		return used_memory_input;
	}

	size_t MemoryManager::getUsedMemoryOutput()
	{
		return used_memory_output;
	}
	
	size_t MemoryManager::getMemory()
	{
		return memory;
	}	
	
	double MemoryManager::getMemoryUsageInput()
	{
		double per;
		if( memory == 0 )
			per = 0;
		else
			per =  2 * ( (double) used_memory_input / (double) ( memory - shared_memory_num_buffers*shared_memory_size_per_buffer ) );
		
		return per;
	}
	
	double MemoryManager::getMemoryUsageOutput()
	{
		double per;
		if( memory == 0 )
			per = 0;
		else
			per =  2 * ( (double) used_memory_output / (double) ( memory - shared_memory_num_buffers*shared_memory_size_per_buffer ) );
		
		return per;
	}
	
	int MemoryManager::getNumBuffersInput()
	{
		return num_buffers_input;
	}
	
	int MemoryManager::getNumBuffersOutput()
	{
		return num_buffers_output;
	}
	
	

}
