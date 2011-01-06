#ifndef _H_MEMORY_MANAGER
#define _H_MEMORY_MANAGER

#include <stdio.h>              // perror
#include <sys/types.h>
#include <sys/ipc.h> 
#include <sys/shm.h> 
#include <map>						// std::map
#include <set>						// std::set
#include <iostream>					// std::cout
#include "samsonDirectories.h"		// SAMSON_SETUP_FILE


#include "Buffer.h"					// ss::Buffer
#include "Lock.h"					// au::Lock
#include "au_map.h"					// au::map

#include "Format.h"					// au::Format
#include "Status.h"				// au::Status

#include "samson.pb.h"			// network::..


#define SS_SHARED_MEMORY_KEY_ID					872934	// the first one

namespace ss {

	/**
	 Memory manager is a singleton implementation of shared memory
	 It should be able to provide buffers of memory to be used across multiple apps
	 */
	
	class SharedMemoryItem 
	{
		
	public:
		int shmid;					/* return value from shmget() */ 
		char *data;					/* Shared memory data */
		size_t size;				/* Information about the size of this shared memory item */
		
		
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
		
		au::Lock lock;									// Lock to be thread-safe

		size_t used_memory;								// Monitor the used memory

		au::map<int,SharedMemoryItem> items;			// Shared memory items

		// Setup parameters ( loaded at constructor )
		
		int num_processes;							// Number of cores to use in this server
		size_t memory;								// Total memory used ( used to limit workers , and free buffered files )

		size_t shared_memory_size_per_buffer;		// Shared memory used in eery buffer
		int shared_memory_num_buffers;				// Number of shared memory buffers to create
		
		bool* shared_memory_used_buffers;			// Bool vector showing if a particular shared memory buffer is used
		
		// Some debug info
		int num_buffers;
		
		MemoryManager();
		
		// Function to create/load the shared memory items
		void createSharedMemoryItems();
		
		// Function used to create a particular shared memory region ( used inside a "Process" to work with its designated area )
		SharedMemoryItem* createSharedMemory( int i );
		
		std::set< Buffer* > buffers;
		
	public:
		
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
		 Get the current usage of memory
		 Used by DiskManager to controll the amount of memory that can use for keeping just writed files on memory
		 */
		
		double getMemoryUsage();
		
		/**
		 Function to request a free shared memory.
		 This is used as output by a ProcessAssistant or as input to prepare a WorkerTaskItem
		 */

		int getFreeSharedMemory()
		{
			lock.lock();
			
			for (int i = 0  ; i < shared_memory_num_buffers ; i++)
				if ( !shared_memory_used_buffers[i] )
				{
					shared_memory_used_buffers[i] = true;
					lock.unlock();
					
					used_memory += shared_memory_size_per_buffer;
					
					return i;
				}
			
			lock.unlock();
			return -1;	// There are no available memory buffers
		}

		
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
		 Function used to free a shared memory used before
		 */

		void freeSharedMemory( int id )
		{
			assert( id >= 0);
			assert( id < shared_memory_num_buffers);
			
			lock.lock();
			shared_memory_used_buffers[id] = false;
			used_memory -= shared_memory_size_per_buffer;
			lock.unlock();
			
		}
		
		/**
		 Interface to get a particular shared memory region
		 */
		
		SharedMemoryItem* getSharedMemory( int i );
		
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
		
	};
	
};

#endif
