

#include "WorkerTaskItem.h"		// Own interface
#include "ProcessAssistant.h"	// ss::ProcessAssistant
#include "WorkerTask.h"			// ss::WorkerTask
#include "WorkerTaskManager.h"	// ss::WorkerTaskManager

namespace ss {


	WorkerTaskItem::WorkerTaskItem(  )
	{
		
		state = definition;			// Initial state of every task item
		shm_input = -1;				// Shared memory area for input files
		num_input_files = 0;		// By default no input files are required
		confirmed_input_files = 0;	// This counter is updated every time an input file is received
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

	std::string WorkerTaskItem::getStatus()
	{
		std::ostringstream output;
		output << "\t\tTask Item (Task: " << task->task_id << ") Item: " << item_id << " ";
		
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
				output << " loading ";
				break;
			case ready_to_load_inputs:
				output << " ready to load inputs ";
				break;
			case running:
				output << " running ";
				break;
		}
		
		return output.str();
	}
	
	
	void WorkerTaskItem::addInputFiles( FileManagerReadItem *item )
	{
		size_t fm_id = FileManager::shared()->addItemToRead( item );
		task->taskManager->pendingInputFiles.insertInMap( fm_id , this );
		num_input_files++;
	}

	void WorkerTaskItem::notifyFinishLoadInputFile()
	{
		confirmed_input_files++;
	}
	
	bool WorkerTaskItem::areInputFilesReady()
	{
		return ( confirmed_input_files == num_input_files);
	}
	

}
