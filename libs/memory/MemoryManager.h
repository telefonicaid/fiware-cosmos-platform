/* ****************************************************************************
 *
 * FILE                     MemoryManager.h
 *
 * AUTHOR                   Andreu Urruela
 *
 * CREATION DATE            2010
 *
 */

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

#include "samson.pb.h"				// network::..


#define SS_SHARED_MEMORY_KEY_ID					872934	// the first one

namespace ss {

	class MemoryRequest;
	
	/*
	 Interface for objects that want to receive the notification about memory request
	 */
	
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
		
		size_t size;							// Required size
		Buffer **buffer;						// Provides buffer
		MemoryRequestDelegate *delegate;		// Delegate to notify when ready
		
		// Reference elements ( to be used in the delegate notification to identify this memory request )
		int component;
		size_t tag;
		size_t sub_tag;
	
		MemoryRequest( size_t _size , Buffer **_buffer, MemoryRequestDelegate *_delegate );
		
		void notifyDelegate()
		{
			assert( delegate );
			delegate->notifyFinishMemoryRequest( this );
		}
		
	};
	
	/*
	 SharedMemoryItem is a class that contains information about a region of memory shared between different processes
	 Memory manager singleton provides pointers to these objects
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
		
		// --------------------------------------------------------------------------------
		// Interfaces to get SimpleBuffer elements in order to read or write to them
		// --------------------------------------------------------------------------------
		
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

	/**
	 
	 Memory manager is a singleton implementation to manager the memory used by any component of SAMSON
	 A unifierd view of the memory is necessary to block some data-generator elements when we are close to the maximum memory
	 It is also responsible to manage shared-memory areas to share between differnt processes. 
	 Shared memory is used to isolate operations from main process
	 
	 */
	
	class MemoryManager 
	{
		
		au::Token token;							// Token to protect "used_memory" and memoryRequests
		au::Stopper stopper;						// Stopper for the main-thread to notify new buffers

		size_t memory;								// Total available memory used
		
		// Monitorization information
		
		size_t used_memory_input;	    			// Memory used for input
		size_t used_memory_output;	    			// Memory used for output

		int num_buffers_input;						// Number of buffers used as inputs
		int num_buffers_output;						// Number of buffers used as outputs
		
		// Setup parameters ( loaded at constructor )
		size_t shared_memory_size_per_buffer;		// Shared memory used in eery buffer
		int shared_memory_num_buffers;				// Number of shared memory buffers to create
		
		bool* shared_memory_used_buffers;			// Bool vector showing if a particular shared memory buffer is used
		
		// List of memory requests
		au::list <MemoryRequest> memoryRequests;	// Only used for inputs
		
		MemoryManager();
		
	public:

		static void init();
		
		/**
		 Singleton interface to the shared memory
		 */
		
		static MemoryManager* shared();

		 /*--------------------------------------------------------------------
		 DIRECT mecanish to request buffers
		 --------------------------------------------------------------------*/
		
		/**
		 Interface to get a buffer of memory
		 Used by networkInterface , DataBuffer and FileManager
		 */
		
		Buffer *newBuffer( std::string name ,  size_t size , Buffer::BufferType type );

		/**
		 Interface to destroy a buffer of memory
		 */
		void destroyBuffer( Buffer *b );
		
		
		/*--------------------------------------------------------------------
		 Delayes mecanish to get memory
		 --------------------------------------------------------------------*/
		
		/**
		 Add a delayed request ( only served when memory is bellow a threshold )
		 */
		
		void addMemoryRequest( MemoryRequest *request );
				
		/**
		 Remove this shared memory. Tipically because we want to change the size...
		 */
		
		/*--------------------------------------------------------------------
		 Shared memory mecanish
		 --------------------------------------------------------------------*/
		
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
		
		/*--------------------------------------------------------------------
		 Get information about memory usage
		 --------------------------------------------------------------------*/
		
		// Fill information
		void fill(network::WorkerStatus*  ws);

		// Get information about current memory usage
		
		size_t getMemory();
		
		size_t getUsedMemory();
		size_t getUsedMemoryInput();
		size_t getUsedMemoryOutput();

		int getNumBuffers();
		int getNumBuffersInput();
		int getNumBuffersOutput();
		
		double getMemoryUsageInput();
		double getMemoryUsageOutput();
		
	public:
		
		// Function for the main thread of memory
		void runThread();
		
	};
	
};

#endif
