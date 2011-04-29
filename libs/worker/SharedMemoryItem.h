/* ****************************************************************************
 *
 * FILE                     SharedMemoryItem.h
 *
 * AUTHOR                   Andreu Urruela
 *
 * CREATION DATE            2010
 *
 */

#ifndef _H_SHARED_MEMORY_ITEM
#define _H_SHARED_MEMORY_ITEM

#include <stdio.h>              // perror
#include <sys/types.h>
#include <sys/ipc.h> 
#include <sys/shm.h> 
#include <map>						// std::map
#include <set>						// std::set
#include <list>						// std::list
#include <iostream>					// std::cout
#include "samsonDirectories.h"		// SAMSON_SETUP_FILE


#include "engine/Buffer.h"					// ss::Buffer
#include "au/Token.h"					// au::Token
#include "au/Stopper.h"				// au::Stopper
#include "au/map.h"					// au::map
#include "au/Format.h"					// au::Format

#include "samson.pb.h"				// network::..


// #define SS_SHARED_MEMORY_KEY_ID	872934	// Not used any more since now, IPC_PRIVATE shared memory can be created

namespace engine {
    
	class MemoryRequest;
	class MemoryRequestDelegate;
    
	
	/*
	 SharedMemoryItem is a class that contains information about a region of memory shared between different processes
	 Memory manager singleton provides pointers to these objects
	 */
	
	class SharedMemoryItem 
	{
		
	public:
		
		int id;						/* Identifier of the shared memory area 0 .. N-1 */
		int shmid;					/* return value from shmget() */ 
		char *data;					/* Shared memory data */
		size_t size;				/* Information about the size of this shared memory item */
		
		SharedMemoryItem( int _id )
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
			if( _size > ( size - offset ) )
                LM_X(1,("Error cheking size of a simple Buffer"));
			return SimpleBuffer( data + offset , _size );
		}
		
	};
    
    	
};

#endif
