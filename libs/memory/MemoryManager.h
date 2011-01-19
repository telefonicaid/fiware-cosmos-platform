#ifndef _H_MEMORY_MANAGER
#define _H_MEMORY_MANAGER

#include <stdio.h>              // perror
#include <sys/types.h>
#include <sys/ipc.h> 
#include <sys/shm.h> 
#include <map>						// std::map
#include <set>						// std::set
#include <list>						// std::list
#include <iostream>					// std::cout
#include "samsonDirectories.h"		// SAMSON_SETUP_FILE


#include "Buffer.h"					// ss::Buffer
#include "Token.h"					// au::Token
#include "Stopper.h"				// au::Stopper
#include "au_map.h"					// au::map
#include "Format.h"					// au::Format
#include "Status.h"					// au::Status

#include "samson.pb.h"				// network::..


#define SS_SHARED_MEMORY_KEY_ID					872934	// the first one

namespace ss {

	class MemoryRequest;
	
	class MemoryRequestDelegate
	{
	public:
		virtual void notifyFinishMemoryRequest( MemoryRequest *request )=0;
	};
	
	/**
	 Object used to request memory for a new operation
	 */
	
	class MemoryRequest
	{
		
	public:
		
		size_t size;						// Required size
		Buffer **buffer;						// Provides buffer
		MemoryRequestDelegate *delegate;	// Delegate to notify when ready
		
		// Reference elements
		int component;
		size_t tag;
		size_t sub_tag;
	
		
		MemoryRequest( size_t _size , Buffer **_buffer,  MemoryRequestDelegate *_delegate )
		{
			size = _size;
			buffer = _buffer;
			delegate = _delegate;	// There is no sence a request for memory with no delegate ;)
		}

		
		void notifyDelegate()
		{
			assert( delegate );
			delegate->notifyFinishMemoryRequest( this );
		}
		
	};
	
	/**
	 Memory manager is a singleton implementation of shared memory
	 It should be able to provide buffers of memory to be used across multiple apps
	 */
	
	class SharedMemoryItem 
	{
		
	public:
		int id;						/* Identifier of the shared memory area */
		int shmid;					/* return value from shmget() */ 
		char *data;					/* Shared memory data */
		size_t size;				/* Information about the size of this shared memory item */
		
		SharedMemoryItem( int _id)
		{
			id = _id;
		}
		
		SimpleBuffer getSimpleBuffer()
		{
			return SimpleBuffer( data , size );
		}
		
		SimpleBuffer getSimpleBufferAtOffset(size_t offset)
		{
			return SimpleBuffer( data + offset , size - offset );
		}
		
		SimpleBuffer getSimpleBufferAtOffsetWithMaxSize(size_t offset , size_t _size)
		{
			assert( _size <= ( size - offset ) );
			return SimpleBuffer( data + offset , _size );
		}
		
	};

		
	class MemoryManager 
	{
		
		au::Token token;							// Token to protect "used_memory"
		au::Stopper stopper;						// Stopper for the main-thread to notify new buffers

		size_t used_memory;							// Monitor the used memory

		// Setup parameters ( loaded at constructor )
		
		int num_processes;							// Number of cores to use in this server
		size_t memory;								// Total memory used ( used to limit workers , and free buffered files )

		size_t shared_memory_size_per_buffer;		// Shared memory used in eery buffer
		int shared_memory_num_buffers;				// Number of shared memory buffers to create
		
		bool* shared_memory_used_buffers;			// Bool vector showing if a particular shared memory buffer is used
		
		// Some debug info
		int num_buffers;
		
		
		// List of memory requests
		au::list <MemoryRequest> memoryRequets;
		
		MemoryManager();
		
		// Buffers in action
		std::set< Buffer* > buffers;
		
	public:

		static void init();
		
		/**
		 Singleton interface to the shared memory
		 */
		
		static MemoryManager* shared();

		/**
		 Interface to get a buffer of memory
		 Used by networkInterface , DataBuffer and FileManager
		 */
		
		Buffer *newBuffer( std::string name ,  size_t size );

		/**
		 Interface to destroy a buffer of memory
		 */
		void destroyBuffer( Buffer *b );
		

		
		/**
		 Add a delayed request ( only served when memory is bellow a threshold )
		 */
		
		void addMemoryRequest( MemoryRequest *request);
		
		/**
		 Get the current usage of memory
		 Used by DiskManager to controll the amount of memory that can use for keeping just writed files on memory
		 */
		
		double getMemoryUsage();
		
		
		/**
		 Remove this shared memory. Tipically because we want to change the size...
		 */
		
		void removeSharedMemory( int i );
		
		/**
		 Function to retain and release a free shared-memory area
		 */

		int retainSharedMemoryArea();
		void releaseSharedMemoryArea( int id );

		/**
		 Functions to get and release a shared memory area
		 */
		
		SharedMemoryItem* getSharedMemory( int i );
		void freeSharedMemory(SharedMemoryItem* item);
		
		/**
		 Debug function to mark as used shared memory areas used by other process
		 */
		void setOtherSharedMemoryAsMarked( int workerId , int num_workers )
		{
			for (int i = 0 ; i < shared_memory_num_buffers ; i++)
			{
				if( (i%num_workers )!= workerId)
					shared_memory_used_buffers[i] = true;
			}
			
		}
		
		
		/**
		 Get a string describing status of memory manager
		 */
		
		void getStatus( std::ostream &output , std::string prefix_per_line );
		
		// Fill information
		void fill(network::WorkerStatus*  ws);
	
		
		size_t get_used_memory()
		{
			return used_memory;
		}
		
		double getUsedMemory()
		{
			return (double)used_memory / (double) memory;
		}
		
		
	public:
		
		// Function for the main thread of memory
		void runThread();
		
	};
	
};

#endif
