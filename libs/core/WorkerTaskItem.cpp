

#include "WorkerTaskItem.h"		// Own interface
#include "ProcessAssistant.h"		// ss::ProcessAssistant

namespace ss {


	WorkerTaskItem::WorkerTaskItem( size_t _task_id , size_t _item_id  , Type _type )
	{
		task_id = _task_id;
		item_id = _item_id;
		
		type = _type;
		
		state = definition;
		
		readItemVector = NULL;	// By default no pending files
		
		shm_input = -1;
	}

	WorkerTaskItem::State WorkerTaskItem::getState()
	{
		return state;
	}

	std::string WorkerTaskItem::getStatus()
	{
		std::ostringstream output;
		output << "\t\tTask Item (Task: " << task_id << ") Item: " << item_id << " ";
		
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
