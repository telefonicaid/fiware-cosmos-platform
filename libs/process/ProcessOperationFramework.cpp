

#include "ProcessOperationFramework.h"		// Own interface
#include "WorkerTaskManager.h"				// ss::WorkerTaskItemWithOutput

namespace ss {

	void ProcessOperationFramework::flushOutput( WorkerTaskItem *taskItem)
	{
		if( pw )
			pw->FlushBuffer(taskItem);
		if( ptw )
			ptw->FlushBuffer(taskItem);
	}

	
	
	
}


