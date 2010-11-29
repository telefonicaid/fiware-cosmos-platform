

#include "WorkerTaskItem.h"		// Own interface
#include "ProcessAssistant.h"	// ss::ProcessAssistant
#include "WorkerTask.h"			// ss::WorkerTask

namespace ss {


	WorkerTaskItem::WorkerTaskItem(  )
	{
		
		state = definition;
		
		readItemVector = NULL;	// By default no pending files
		
		shm_input = -1;
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
			case ready:
				output << " ready ";
				break;
			case no_memory:
				output << " waiting available memory ";
				break;
			case loading:
				output << " loading ";
				break;
			case running:
				output << " running ";
				break;
		}
		
		if( readItemVector )
		{
			output << "[Pending files.. " << readItemVector->getStatus() << "]";
		}
		
		return output.str();
	}

}
