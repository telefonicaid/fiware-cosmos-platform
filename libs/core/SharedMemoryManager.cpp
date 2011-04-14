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
#include "Format.h"             // au::Format

namespace ss
{
    
    SharedMemoryManager *sharedMemoryManager = NULL;    // Unique shared memory manager

	
    // Init the statis shared memory manager
    //SharedMemoryManager *SharedMemoryManager::sharedMemoryManager = NULL;
    
    void destroy_SharedMemoryManager()
    {
        delete sharedMemoryManager;
        sharedMemoryManager = NULL;
    }
    
    
    void SharedMemoryManager::init()
    {
        // Init the global shard memory element ( after init samsonSetup )
        sharedMemoryManager = new SharedMemoryManager(); 
        
        atexit(destroy_SharedMemoryManager);
    }
    
    SharedMemoryManager* SharedMemoryManager::shared()
    {
        if( !sharedMemoryManager )
            LM_X(1,("SharedMemoryManager was not initialized"));
        
        return sharedMemoryManager;
    }
    
    
	SharedMemoryManager::SharedMemoryManager()
	{
		
		// Shared memory setup
		shared_memory_size_per_buffer	= SamsonSetup::shared()->shared_memory_size_per_buffer;
		shared_memory_num_buffers		= SamsonSetup::shared()->num_processes;
		
        sharedMemoryIdsFileName =  SamsonSetup::shared()->baseDirectory + "/shared_memory_ids.data";
        
		if( shared_memory_size_per_buffer == 0)
			LM_X(1,("Error in setup, invalid value for shared memory size %u", shared_memory_size_per_buffer ));
		
		// Boolean vector showing if a buffer is used
		shared_memory_used_buffers = (bool*) malloc( shared_memory_num_buffers * sizeof(bool));

		for (int i = 0 ; i < shared_memory_num_buffers ; i++)
			shared_memory_used_buffers[i] = false;

        // Create the shared memory segments
        createSharedMemorySegments( );		
        
		LM_T(LmtMemory , ("SharedMemoryManager init %d shared memory areas", shared_memory_num_buffers ) );
        
        atexit(destroy_SharedMemoryManager);
        
	}
	
	SharedMemoryManager::~SharedMemoryManager()
	{
        // Remove the shared memory segments
        removeSharedMemorySegments(shm_ids, shared_memory_num_buffers);
        
        // Remove the file since it will not be necessary
        remove( sharedMemoryIdsFileName.c_str() );  
    
		// Free the vector of flags for shared memory areas
		free(shared_memory_used_buffers);
        free( shm_ids );

	}
	
#pragma mark ----

    void SharedMemoryManager::createSharedMemorySegments( )
    {
        
        // First try to remove the previous shared memory segments ( if any )
        
        // Get the size of the file ( if exist )
        {
            size_t fileSize = au::Format::sizeOfFile( sharedMemoryIdsFileName );
            
            int length = fileSize / sizeof(int);
            int *ids = (int*) malloc( length * sizeof(int) );
            if( !ids )
                LM_X(1,("Malloc returned NULL"));
            
            FILE *file = fopen( sharedMemoryIdsFileName.c_str() , "r" );
            if( file )
            {
                if( fread(ids, length * sizeof(int) , 1, file) == 1 )
                {
                    LM_M(("Removing previous memory segments"));
                    removeSharedMemorySegments(ids, length);
                }
                fclose(file);
            }
            
            free( ids );
        }
        
        // Create a new shared memory buffer starting at SS_SHARED_MEMORY_KEY_ID + start_id
        // The return value means that a new shared memory buffer is created with if  SS_SHARED_MEMORY_KEY_ID + returned_value
        
        shm_ids = (int*) malloc( sizeof(int) * shared_memory_num_buffers );
        
        LM_M(("Creating shared memory buffers"));
        
        for (int i = 0 ; i < shared_memory_num_buffers ; i++ )
        {
            int shmflg;		/* shmflg to be passed to shmget() */ 
            
            //shmflg  = 384;			// Permission to read / write ( only owner )
            shmflg  = 384;			// Permission to read / write ( only owner )
            
            int shmid = shmget(IPC_PRIVATE , shared_memory_size_per_buffer, shmflg);
            
            if( shmid == -1)
            {
                perror("shmid");
                LM_E(("Error creating the shared memory buffer ( %d / %d ). Please review SAMSON documentation about shared memory usage",
					  i ,shared_memory_num_buffers ));
#ifdef __LP64__
                LM_X(1, ("shmid  (%s)", strerror(errno)));
#endif
            }
            
            shm_ids[i] = shmid;
        }
    
        {
            FILE *file = fopen( sharedMemoryIdsFileName.c_str() , "w" );
            if( file )
            {
                if( !fwrite(shm_ids, shared_memory_num_buffers * sizeof(int), 1, file) == 1)
                    LM_W(("Not possible to write shared memory segments on file %s. This could be a problem if the app crashes." , sharedMemoryIdsFileName.c_str()));
                
                fclose(file);
            }
            else
                LM_W(("Not possible to write shared memory segments on file %s. This could be a problem if the app crashes." , sharedMemoryIdsFileName.c_str()));
        }
        
    }
    
    
    void SharedMemoryManager::removeSharedMemorySegments( int * ids , int length )
    {
        LM_M(("Releasing %d shared memory buffers", length ));
        
        for ( int i = 0 ; i < length ; i++)
        {
            // Remove the shared memory areas
            if( shmctl( ids[i] , IPC_RMID , NULL ) == -1 )
                LM_W(("Error trying to release a shared memory buffer Please review shared-memory problems in SAMSON documentation"));
        }
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
	
    void SharedMemoryManager::fill( network::WorkerStatus* ws)
    {
        std::ostringstream output;
        
        int used_shared_memory_num_buffers = 0;
        for (int i = 0 ; i < shared_memory_num_buffers ; i++)
            if( shared_memory_used_buffers[i] )
                used_shared_memory_num_buffers++;
        
        output << au::Format::string("Size: %s Used %d/%d", 
                                     au::Format::string(shared_memory_size_per_buffer).c_str() , 
                                     used_shared_memory_num_buffers , 
                                     shared_memory_num_buffers);
        
        ws->set_shared_memory_status( output.str() );
        
    }
    
    

}
