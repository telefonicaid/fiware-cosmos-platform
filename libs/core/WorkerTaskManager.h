#ifndef _H_WORKER_TASK_MANAGER
#define _H_WORKER_TASK_MANAGER

#include "samson.pb.h"


namespace ss {

	class SamsonWorker;
	
	class WorkerTaskManager
	{
		SamsonWorker *worker;
		
	public:
		
		WorkerTaskManager(SamsonWorker *_worker)
		{
			worker = _worker;
		}

		// Add a task from network interface
		// Return true if the task is already finished within this call ( not schedulled in taskManager )
		bool addTask( const network::WorkerTask &task );
	
		// Fill information about status of this worker
		void fillWorkerStatus( network::WorkerStatus* status );
		
	};

}

#endif
