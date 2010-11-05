

#include "ControllerTask.h"			// Own interface
#include "JobManager.h"				// ss::JobManager

namespace ss {

	void ControllerTask::notifyWorkerConfirmation( int worker_id , network::WorkerTaskConfirmation confirmationMessage )
	{
		// Increment the number of workers that confirmed this task
		confirmed_workers++;
	}
	
}

