

#include "ProcessOperationFramework.h"		// Own interface
#include "WorkerTaskManager.h"				// ss::WorkerTaskItemWithOutput

namespace ss {

	void ProcessOperationFramework::flushOutput( WorkerTaskItemWithOutput *taskItem)
	{
		pw->FlushBuffer(taskItem);
	}
	
}


