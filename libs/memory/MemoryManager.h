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


#include "Buffer.h"            // ss::Buffer
#include "Lock.h"              // au::Lock



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
		
	};

	
	// Setup information
	
	class MemoryManagerSetup
	{
		
	public:
		
		int num_cores;
		size_t shared_memory_per_core;

		MemoryManagerSetup()
		{
			// Load from file...
			num_cores = 2;
			shared_memory_per_core = 536870912;
			
			FILE *file = fopen( SAMSON_SETUP_FILE , "r" );
			char line[2000];
			if( file )
			{
				
				while (fgets(line,2000,file)) 
				{
					au::CommandLine c;
					c.parse(line);
					
					// Skip comments
					if( c.get_num_arguments() == 0)
						continue;
					if( c.get_argument(0).c_str()[0] == '#' )
						continue;

					
					if( c.get_num_arguments() < 2)
						continue;
					
					if( c.get_argument(0) == "shm_size_per_core" )
						shared_memory_per_core = atoll( c.get_argument(1).c_str() );
				}
				
			}
			
		}
	};
	
	class MemoryManager
	{
		
		MemoryManager();
		au::Lock lock;			// Lock to be thread-safe

		size_t used_memory;		// Monitor the used memory

		// Shared memory items
		std::map<int,SharedMemoryItem*> items;
		
		// Setup parameters
		MemoryManagerSetup setup;
		
		
		// Function to create/load the shared memory items
		void createSharedMemoryItems()
		{
			// Create shared memory
			for (int i = 0 ; i < setup.num_cores ;i++)
				createSharedMemory(i);
		}
		
		SharedMemoryItem* createSharedMemory( int i )
		{
			lock.lock();
			
			SharedMemoryItem* _info = new SharedMemoryItem();
			
			key_t key;		/* key to be passed to shmget() */ 
			int shmflg;		/* shmflg to be passed to shmget() */ 
			size_t size;	/* size to be passed to shmget() */ 
			
			
			key = SS_SHARED_MEMORY_KEY_ID + i; 
			size = setup.shared_memory_per_core;
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
		
		
	public:
		static MemoryManager* shared();
	
		// Interface to get a buffer of memory
		Buffer *newPrivateBuffer( size_t size );
		
		// Interface to desply a buffer of memory
		void destroy( Buffer *b);

		
		SharedMemoryItem* getSharedMemory( int i )
		{
			std::map<int,SharedMemoryItem*>::iterator s = items.find( i );	
			if( s == items.end() )
				return createSharedMemory(i);
			else
				return s->second;
		}

		

		
		
	};
	
};

#endif
