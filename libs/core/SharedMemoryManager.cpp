/* ****************************************************************************
*
* FILE                     SharedMemoryManager.cpp
*
* AUTHOR                   Andreu Urruela
*
* CREATION DATE            2010
*
*/

#include "logMsg.h"             // LM_*
#include "traceLevels.h"        // Trace Levels

#include "SharedMemoryManager.h"		// Own interface
#include "Buffer.h"				// ss::Buffer
#include "SamsonSetup.h"		// ss:SamsonSetup
#include <sstream>				// std::stringstream
#include "MemoryRequest.h"		// ss::MemoryRequest
#include "Engine.h"				// ss::Engine
#include "SharedMemoryItem.h"   // ss::SharedMemoryItem


namespace ss
{
	
	SharedMemoryManager::SharedMemoryManager()
	{
		
		// Shared memory setup
		shared_memory_size_per_buffer	= SamsonSetup::shared()->shared_memory_size_per_buffer;
		shared_memory_num_buffers		= SamsonSetup::shared()->num_processes;
		
		if( shared_memory_size_per_buffer == 0)
			LM_X(1,("Error in setup, invalid value for shared memory size %u", shared_memory_size_per_buffer ));
		
		// Boolean vector showing if a buffer is used
		shared_memory_used_buffers = (bool*) malloc( shared_memory_num_buffers * sizeof(bool ) );
        shm_ids = (int*) malloc( sizeof(int) * shared_memory_num_buffers );

		for (int i = 0 ; i < shared_memory_num_buffers ; i++)
			shared_memory_used_buffers[i] = false;

        // Create the shared memory areas as private
        createNewSharedMemoryBuffers();		
        
		LM_T(LmtMemory , ("SharedMemoryManager init %d shared memory areas", shared_memory_num_buffers ) );
        
	}
	
	SharedMemoryManager::~SharedMemoryManager()
	{
        for ( int i = 0 ; i < shared_memory_num_buffers ; i++)
        {
            // Remove the shared memory areas
            LM_M(("Releasing shared memory buffers"));
            if( shmctl( shm_ids[i] , IPC_RMID , NULL ) == -1 )
                LM_W(("Not releasing a shared memory area. Please review shared-memory problems in SAMSON documentation"));
                
        }
                                                        
        
		// Free the vector of flags for shared memory areas
		free(shared_memory_used_buffers);
        free( shm_ids );
	}
	
	
	int SharedMemoryManager::retainSharedMemoryArea()
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
	
	
	
	void SharedMemoryManager::releaseSharedMemoryArea( int id )
	{
		if( (id < 0) || ( id > shared_memory_num_buffers) )
			LM_X(1, ("Releaseing a wrong Shared Memory Id %d",id));
		
		token.retain();
		
		shared_memory_used_buffers[id] = false;
		
		token.release();
		
	}	
	
	
    void SharedMemoryManager::createNewSharedMemoryBuffers( )
    {
        // Create a new shared memory buffer starting at SS_SHARED_MEMORY_KEY_ID + start_id
        // The return value means that a new shared memory buffer is created with if  SS_SHARED_MEMORY_KEY_ID + returned_value

        LM_M(("Creating shared memory buffers"));

        for (int i = 0 ; i < shared_memory_num_buffers ; i++ )
        {
            int shmflg;		/* shmflg to be passed to shmget() */ 
            size_t size;	/* size to be passed to shmget() */ 
            
            size    = shared_memory_size_per_buffer;
            shmflg  = 384;			// Permission to read / write ( only owner )
            
            int shmid = shmget(IPC_PRIVATE , size, shmflg);
            
            if( shmid == -1)
            {
                perror("shmid");
                LM_E(("Error creating the shared memory buffers. Please review SAMSON documentation about shared memory usage"));
                LM_X(1, ("shmid  (%s)", strerror(errno)));
            }
            
            shm_ids[i] = shmid;
        }
        
    }
    
	SharedMemoryItem* SharedMemoryManager::getSharedMemory( int i )
	{
        
		// Create a new shared memory area
		SharedMemoryItem* _info = new SharedMemoryItem(i);
		_info->shmid = shm_ids[i];
        
		// Attach to local-space memory
		_info->data = (char *)shmat(_info->shmid, 0, 0);
		if( _info->data == (char*)-1 )
		{
			perror("shmat: shmat failed"); 
			LM_X(1, ("Error with shared memory while attaching to local memory (shared memory id %d )\n",i));
		}
		
		_info->size = shared_memory_size_per_buffer;
		
		return _info;		
	}
	
	void SharedMemoryManager::freeSharedMemory(SharedMemoryItem* item)
	{
		if (item == NULL)
			LM_RVE(("NULL SharedMemoryItem as input ..."));
        
		// Detach data
		int ans = shmdt( item->data );
		
		// Make sure operation is correct
		if( ans == -1)
			LM_X(1,("Error calling shmdt"));
		
		// remove the item created with getSharedMemory
		delete item;
	}
	

}
