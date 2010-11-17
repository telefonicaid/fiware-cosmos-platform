#ifndef _H_MEMORY_MANAGER
#define _H_MEMORY_MANAGER

#include <stdio.h>              // perror
#include <sys/types.h>
#include <sys/ipc.h> 
#include <sys/shm.h> 
#include <map>						// std::map
#include <iostream>					// std::cout
#include "samsonDirectories.h"		// SAMSON_SETUP_FILE
#include "CommandLine.h"			// au::CommandLine


#include "Buffer.h"					// ss::Buffer
#include "Lock.h"					// au::Lock
#include "au_map.h"					// au::map

#include "Format.h"					// au::Format

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
	};

		
	class MemoryManager
	{
		
		au::Lock lock;									// Lock to be thread-safe
		au::StopLock stopLock;							// Stop lock for threads waiting for memory ( Process )

		size_t used_memory;								// Monitor the used memory

		au::map<int,SharedMemoryItem> items;			// Shared memory items
		
		// Setup parameters ( loaded at constructor )
		
		int num_processes;							// Number of cores to use in this server
		size_t shared_memory_per_process;			// Shared memory used in per-core operation
		size_t memory;								// Total memory used ( used to limit workers , and free buffered files )

		// Some debug info
		int num_buffers;
		
		MemoryManager();
		
		// Function to create/load the shared memory items
		void createSharedMemoryItems();
		
		// Function used to create a particular shared memory region ( used inside a "Process" to work with its designated area )
		SharedMemoryItem* createSharedMemory( int i );
		
	public:
		
		/**
		 Singleton interface to the shared memory
		 */
		
		static MemoryManager* shared();

		/**
		 Interface to get a buffer of memory
		 Used by networkInterface , DataBuffer and FileManager
		 */
		
		Buffer *newBuffer( size_t size );

		/**
		 Interface to destroy a buffer of memory
		 */
		void destroyBuffer( Buffer *b );

		
		/** 
		 Interface to get a buffer only if used memory is bellow a particular percentadge.
		 If that is not the case, this call is blocking until the memory is free enougth
		 This is used when Process produce data. Only if enougth memory is available, data is send to the network interface
		 */
		 
		 Buffer *newBufferIfMemoryBellow( size_t size , double max_usage_memory , bool blocking );

		
		/**
		 Get the current usage of memory
		 Used by DiskManager to controll the amount of memory that can use for keeping just writed files on memory
		 */
		
		double getMemoryUsage();
		
		/**
		 Interface to get a particular shared memory region
		 */
		
		SharedMemoryItem* getSharedMemory( int i );
		
		/**
		 Get a string describing status of memory manager
		 */
		
		std::string str()
		{
			int per_memory = (int) getMemoryUsage()*100;
			
			std::ostringstream o;
			o << "Memory Manager" << std::endl;
			o << "=========================" << std::endl;
			o << "Used memory: " << au::Format::string( used_memory ) << " / " << au::Format::string(memory) << " (" << per_memory << "%)"<< std::endl;
			o << "Number of buffers in action " << num_buffers << std::endl;
			
			return o.str();
		}
		
		std::string getStatus()
		{
			int per_memory = (int) getMemoryUsage()*100;
			std::ostringstream o;
			o << "Used: " << au::Format::string( used_memory ) << " / " << au::Format::string(memory) << " (" << per_memory << "%)";
			o << " #Buffers " << num_buffers;
			return o.str();
		}
		
		
	};
	
};

#endif
