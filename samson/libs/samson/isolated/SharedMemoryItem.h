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


#include "au/mutex/Token.h"					// au::Token
#include "au/containers/map.h"					// au::map
#include "au/string.h"					// au::Format

#include "engine/Buffer.h"					// samson::Buffer

#include "samson/common/samsonDirectories.h"		// SAMSON_SETUP_FILE
#include "samson/common/samson.pb.h"				// network::..


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
		
		SharedMemoryItem( int _id , int _shmid , size_t _size )
		{
			id = _id;
      shmid = _shmid;
      size = _size;
      
      // Attach to local-space memory
      data = (char *) shmat( shmid, 0, 0 );
      if( data == (char*)-1 )
        LM_X(1, ("Error with shared memory while attaching to local memory ( %s )( shared memory id %d shmid %d size %lu )\n"
                 , strerror(errno) ,  id , shmid , size ));
		}
		
    ~SharedMemoryItem()
    {
      // Detach data if it was previously attached
      if( data )
      {
        if( shmdt( data ) == -1 )
          LM_X(1,("Error calling shmdt"));
      }
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
