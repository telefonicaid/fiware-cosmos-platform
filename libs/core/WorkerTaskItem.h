

#ifndef _H_WORKER_TASK_ITEM
#define _H_WORKER_TASK_ITEM

#include <cstring>		// size_t		
#include <assert.h>		// assert(.)
#include <string>		// std::string
#include <sstream>		// std::ostringstream
#include "FileManager.h"		// ss::FileManager ( and derivates )
#include "samson.pb.h"	// ss::network::...

namespace ss {

	class ProcessAssistant;
	class WorkerTask;
	
	class WorkerTaskItem
	{
	public:
		
		WorkerTask *task;		// Pointer to the parent task
		
		enum State
		{
			definition,			// Defining the task
			no_memory,			// Waiting for a shared memory item to be assigned
			loading,			// Loading data from disk
			ready,				// Ready to be process by any ProcessAssistant
			running				// It has beent taken by a ProcessAssistant (should report finish)
		};
		
		int item_id;		// Item id ( from 0 to ...)
		
		State state;

		int shm_input;	// Shared memory identifier for input ( if necessary ) -1 --> no assigned
		
		WorkerTaskItem();
		virtual ~WorkerTaskItem(){}

		// Set the parent task
		void setTaskAndItemId( WorkerTask *task , int itemId);
		
		FileManagerReadItemVector *readItemVector;	// elements that need to be read to run this item
		
		void setup()
		{
			if ( state == running )
				return;
			
			if ( state == definition )
			{
				if( readItemVector == NULL)
					state = ready;				// No input
				else
				{
					// ask for memory
					shm_input = MemoryManager::shared()->getFreeSharedMemory();
					if( shm_input == -1)
					{
						state = no_memory;
						return;
					}
					else
					{
						SharedMemoryItem *item = MemoryManager::shared()->getSharedMemory( shm_input ); 
						readItemVector->setBuffer( item->data , item->size );
						
						state = loading;
						FileManager::shared()->addItemsToRead( readItemVector );	
						return;
					}
				}
			}
			
			if( state == no_memory )
			{
				// ask for memory
				shm_input = MemoryManager::shared()->getFreeSharedMemory();
				if( shm_input == -1)
				{
					state = no_memory;
					return ;
				}
				else
				{
					state = loading;
					FileManager::shared()->addItemsToRead( readItemVector );	
					return;
				}
				
			}
			
			if( state == loading)
			{
				if( readItemVector->isReady() )
					state = ready;
			}
		}
		
		void freeResources()
		{
			if( shm_input != -1)
				MemoryManager::shared()->freeSharedMemory( shm_input );
		}
		
		
		State getState();
		
		bool isReadyToRun()
		{
			if( state == ready )
			{
				state = running;
				return true;
			}
			
			return false;
			
		}
		
		// Get status for debuggin
		std::string getStatus();
		
		// Particular operations to run
		virtual void run(ProcessAssistant *pa)=0;
		virtual void receiveCommand( ProcessAssistant *pa , network::ProcessMessage p )=0;
		
	};
}
#endif
