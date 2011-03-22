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
#include "MemoryRequest.h"		// ss::MemoryRequest
#include "EngineDelegates.h"	
#include "Engine.h"				// ss::Engine

namespace ss
{
	
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
		
		if( shared_memory_size_per_buffer == 0)
			LM_X(1,("Error in setup, invalid value for shared memory size %u", shared_memory_size_per_buffer ));
		
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
		
		LM_T(LmtMemory , ("MemoryManager init with %s and %d shared memory areas", au::Format::string( memory , "B").c_str() , shared_memory_num_buffers ) );
	}
	
	MemoryManager::~MemoryManager()
	{
		// Free the vector of flags for shared memory areas
		free(shared_memory_used_buffers);
	}
	
	
	void MemoryManager::removeSharedMemory( int i )
	{
		key_t key;		/* key to be passed to shmget() */ 
		int shmflg;		/* shmflg to be passed to shmget() */ 
		size_t size;	/* size to be passed to shmget() */ 
		
		key = SS_SHARED_MEMORY_KEY_ID + i; 
		shmflg = IPC_CREAT | 384;			// Permission to read / write ( only owner )
		size = 1;							// Make sure it is does not return an error since the size is to large
		
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
		
		key_t key;		/* key to be passed to shmget() */ 
		int shmflg;		/* shmflg to be passed to shmget() */ 
		size_t size;	/* size to be passed to shmget() */ 
		
		int shmid;		// Result of shmget
		
		key = SS_SHARED_MEMORY_KEY_ID + i; 
		size = shared_memory_size_per_buffer;
		shmflg = IPC_CREAT | 384;			// Permission to read / write ( only owner )
		
		if ((shmid = shmget(key, size, shmflg)) == -1)
		{
			// Remove it
			removeSharedMemory(i);

			// Second try
			if ((shmid = shmget(key, size, shmflg)) == -1)
				LM_X(1, ("shmget: %s", strerror(errno)));
				// LM_RE(NULL, ("shmget: %s", strerror(errno)));
		}

		//
		SharedMemoryItem* _info = new SharedMemoryItem(i);
		_info->shmid = shmid;
		
		// Attach to local-space memory
		_info->data = (char *)shmat(_info->shmid, 0, 0);
		if( _info->data == (char*)-1 )
		{
			perror("shmat: shmat failed"); 
			LM_X(1, ("Error with shared memory while attaching to local memory (shared memory id %d )\n",i));
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
		Buffer *b = _newBuffer( name , size , type );
		token.release();
		
		return b;
	}

	Buffer *MemoryManager::_newBuffer( std::string name , size_t size , Buffer::BufferType type )
	{
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
		return b;
	}	
	
	void MemoryManager::destroyBuffer(Buffer* b)
	{
		if (b == NULL)
			return;

		token.retain();
		
		LM_T(LmtMemory, ("Destroying buffer with max size %sbytes", au::Format::string( b->getMaxSize() ).c_str() ) );
		
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
		

		LM_T(LmtMemory, ("[DONE] Destroying buffer with max size %sbytes", au::Format::string( b->getMaxSize() ).c_str() ) );
		
		
		b->free();
		delete b;
		
		token.release();

		// Check requests to schedule new notifications
		checkMemoryRequests();
		
		// Check process halted in the Engine
		Engine::shared()->checkBackgroundProcesses();
		
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
		
		LM_X(1,("Error since there are no available shared memory buffers"));
		return -1;	// There are no available memory buffers, so we will never get this point
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

		output << "Input: " << ((int)(getMemoryUsageInput()*100.0)) << "% "; 
		output << "Output: " << ((int)(getMemoryUsageOutput()*100.0)) << "% "; 
		
		output << "[ Input: " << num_buffers_input << " buffers with " << au::Format::string( used_memory_input , "B" );
		output << " Output: " << num_buffers_output << " buffers with " << au::Format::string( used_memory_output , "B" ) << " ]";
		
		ws->set_memory_status( output.str() );
		ws->set_total_memory( memory );
		ws->set_used_memory( getUsedMemory() );
	}
	
	void MemoryManager::addMemoryRequest( MemoryRequest *request)
	{
		
		if( request->size > memory )
			LM_X(-1,("Error managing memory: excesive memory request"));	

		LM_T( LmtMemory , ("Adding memory request for %s" , au::Format::string( request->size , "B" ).c_str() ));
		
		token.retain();
		memoryRequests.push_back( request );
		token.release();

		LM_T( LmtMemory , ("[DONE] Adding memory request for %s" , au::Format::string( request->size , "B" ).c_str() ));
		
		checkMemoryRequests();
	}
	
	 
	// Function to check memory requests and notify using Engine if necessary
	void MemoryManager::checkMemoryRequests()
	{
		LM_T( LmtMemory , ("Checking memory requests Pending requests %u" , memoryRequests.size() ));
		
		token.retain();
		
		while( true )
		{
			double p = getMemoryUsageInput();	// Only used for inputs

			MemoryRequest *r = NULL;
				
			if ( p < 1.0 )
			{
				r = memoryRequests.extractFront();
			}

			if( !r )
			{
				// Nothing to notify
				break;
			}
			else
			{
				*(r->buffer) = _newBuffer("Buffer from request", r->size , Buffer::input);
				Engine::shared()->add( new MemoryRequestNotification( r ) );
			}
			
		}
		
		token.release();
		
		LM_T( LmtMemory , ("[DONE] Checking memory requests Pending requests %u" , memoryRequests.size() ));
		
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

	size_t MemoryManager::getMemoryOutput()
	{
		return ( memory - shared_memory_num_buffers*shared_memory_size_per_buffer )/2;
	}
	
	size_t MemoryManager::getMemoryInput()
	{
		return ( memory - shared_memory_num_buffers*shared_memory_size_per_buffer )/2;
	}
	
	
	bool MemoryManager::availableMemoryOutput()
	{
		return ( getMemoryUsageOutput() < 1.0 );
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
