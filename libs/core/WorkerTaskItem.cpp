

#include "WorkerTaskItem.h"		// Own interface
#include "ProcessAssistant.h"	// ss::ProcessAssistant
#include "WorkerTask.h"			// ss::WorkerTask
#include "WorkerTaskManager.h"	// ss::WorkerTaskManager

namespace ss {
	
	WorkerTaskItem::WorkerTaskItem( const network::WorkerTask &_workerTask )
	{
		workerTask = _workerTask;

		state = definition;			// Initial state of every task item
		shm_input = -1;				// Shared memory area for input files
		
		// Init the input system
		num_input_files = 0;
		confirmed_input_files = 0;
		
		// No error by default
		error = false;
		
	}

	
	void WorkerTaskItem::setTaskAndItemId( WorkerTask *_task , int _itemId )
	{
		task = _task;
		item_id = _itemId;
	}

	WorkerTaskItem::State WorkerTaskItem::getState()
	{
		return state;
	}

	// Funciton to get the run-time status of this object
	void WorkerTaskItem::getStatus( std::ostream &output , std::string prefix_per_line )
	{
		output << "Item " << item_id <<  " (Task: " << task->task_id << ") ";
		output << "[SHM: " << shm_input << "] ";
		switch (state) {
			case definition:
				output << " in definition ";
				break;
			case ready_to_run:
				output << " ready to run ";
				break;
			case no_memory:
				output << " waiting available memory ";
				break;
			case loading_inputs:
				output << " loading [" << confirmed_input_files << "/" << num_input_files << "]";
				break;
			case ready_to_load_inputs:
				output << " ready to load inputs ";
				break;
			case running:
				output << " running ";
				break;
		}
		
		output << "\n";
	}
	
	
	void WorkerTaskItem::addInputFiles( FileManagerReadItem *item )
	{
		item->setDelegate( task->taskManager );	// Make sure task manager is the delegate for all inputs 
		size_t fm_id = FileManager::shared()->addItemToRead( item );
		task->taskManager->addInputFile( fm_id , this );
		num_input_files++;
	}

	void WorkerTaskItem::notifyFinishLoadInputFile()
	{
		confirmed_input_files++;
		
		if( areInputFilesReady() )
			state = ready_to_run;
		
	}
	
	bool WorkerTaskItem::areInputFilesReady()
	{
		return ( confirmed_input_files == num_input_files);
	}
	

	bool WorkerTaskItem::isReadyToRun()
	{
		return( state == ready_to_run );
	}
	
	void WorkerTaskItem::setError( std::string _error_message )
	{
		error = true;
		error_message = _error_message;
	}
	
	void WorkerTaskItem::start()
	{
		assert( state == ready_to_run);
		state = running;
	}
	
	void WorkerTaskItem::setup()
	{
		if( state == no_memory )
		{
			shm_input = MemoryManager::shared()->getFreeSharedMemory();
			if( shm_input != -1)
				state = ready_to_load_inputs;
		}
		
		if( state == ready_to_load_inputs)
		{
			setupInputs();					// Prepare all the inputs to be read
			
			if( areInputFilesReady() )		// Theoretically, if no input is finally scheduled
				state = ready_to_run;
			else
				state = loading_inputs;
		}
		
	}
	
	void WorkerTaskItem::freeResources()
	{
		if( shm_input != -1)
			MemoryManager::shared()->freeSharedMemory( shm_input );
	}
	
	network::Queue WorkerTaskItem::getOutputQueue(int o)
	{
		assert( workerTask.output_size() > o );
		return workerTask.output(o);
	}
	
	
	FileManagerReadItem * WorkerTaskItem::getFileMangerReadItem( ProcessAssistantSharedFile* file  )
	{
		// Read the key-value information for each hash group for each input files
		size_t offset			= sizeof( FileHeader );					// We skip the file header
		size_t size				= sizeof(FileKVInfo) * KV_NUM_HASHGROUPS;
		
		FileManagerReadItem *item = new FileManagerReadItem( file->fileName , offset , size , (char *) file->info , NULL );
		return item;
	}	
	
	
}
