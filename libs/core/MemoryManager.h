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
	class MemoryRequestDelegate;

	
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
		
		au::Token token;							// Token to protect this instance and memoryRequests
													// It is necessary to protect since network thread can access directly here

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
		
		
	public:
		
		MemoryManager();
		~MemoryManager();

		 /*--------------------------------------------------------------------
		 DIRECT mecanish to request buffers
		 --------------------------------------------------------------------*/
		
		/**
		 Interface to get a buffer of memory
		 Used by networkInterface , DataBuffer and FileManager
		 */
		
		Buffer *newBuffer( std::string name ,  size_t size , Buffer::BufferType type );

	private:
		
		Buffer *_newBuffer( std::string name ,  size_t size , Buffer::BufferType type );

	public:
		
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
		 Check if a new memory request can be accepted.
		 If possible, a new element is added to Engine to notify
		 */
		
		void checkMemoryRequests();
		
		
	public:
		
		
		/*--------------------------------------------------------------------
		 Shared memory mecanish
		 --------------------------------------------------------------------*/
		
		
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
		void removeSharedMemory( int i );
		
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

		size_t getMemoryInput();
		size_t getMemoryOutput();
		
		int getNumBuffers();
		int getNumBuffersInput();
		int getNumBuffersOutput();
		
		double getMemoryUsageInput();
		double getMemoryUsageOutput();

		// Informs about the usage of the output memory ( it is used to block data generators if necessary )
		bool availableMemoryOutput();
		
	public:
		
		// Function for the main thread of memory
		void runThread();
		
	};
	
};

#endif
