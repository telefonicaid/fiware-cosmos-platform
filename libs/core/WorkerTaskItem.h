

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
			definition,				// Defining the task
			no_memory,				// Waiting for a shared memory item to be assigned
			ready_to_load_inputs,	// Ready to load data from inputs
			loading_inputs,			// Loading data from disk
			ready_to_run,			// Ready to be process by any ProcessAssistant
			running					// It has beent taken by a ProcessAssistant (should report finish)
		};
		
		int item_id;			// Item id ( from 0 to ...)
		
		State state;

		int shm_input;			// Shared memory identifier for input ( if necessary ) -1 --> no assigned
		
		WorkerTaskItem();
		virtual ~WorkerTaskItem(){}

		// Set the parent task
		void setTaskAndItemId( WorkerTask *task , int itemId);

		int num_input_files;
		int confirmed_input_files;
		
		virtual void setupInputs(){}	// Prepare files needed to be read when shared memory is ready
		void addInputFiles( FileManagerReadItem *item );
		void notifyFinishLoadInputFile();
		bool areInputFilesReady();
		
		// General setup while asking if it is ready
		
		bool isReadyToRun()
		{
			if ( state == running )
				return false;				// Already running by other PA
			
			if( state == no_memory )
			{
				shm_input = MemoryManager::shared()->getFreeSharedMemory();
				if( shm_input != -1)
					state = ready_to_load_inputs;
			}

			if( state == ready_to_load_inputs)
			{
				setupInputs();	// Prepare all the inputs to be read
				state = loading_inputs;
			}	
			
			if ( state == loading_inputs )
			{
				if( areInputFilesReady() )
					state = ready_to_run;
			}

			if( state == ready_to_run )
			{
				state = running;
				return true;
			}
			
			return false;

		}
		
		
		void freeResources()
		{
			if( shm_input != -1)
				MemoryManager::shared()->freeSharedMemory( shm_input );
		}
		
		
		State getState();
		
		
		// Get status for debuggin
		std::string getStatus();
		
		// Particular operations to run
		virtual void run(ProcessAssistant *pa)=0;
		virtual void receiveCommand( ProcessAssistant *pa , network::ProcessMessage p )=0;
		
	};
}
#endif
