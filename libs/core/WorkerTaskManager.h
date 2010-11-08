#ifndef _H_WORKER_TASK_MANAGER
#define _H_WORKER_TASK_MANAGER

#include "SamsonWorker.h"        // SamsonWorker
#include "samson.pb.h"           // WorkerTask



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
		// void fillWorkerStatus( network::WorkerStatus* status );
		
		
		/*
		// Send a notification for a particular task to the controller
		void sendConfirmation( size_t task_id )
		{
			// Send a confirmation just to test everything is ok
			Packet                            p;
			network::WorkerTaskConfirmation*  confirmation = p.message.mutable_worker_task_confirmation();
			confirmation->set_task_id(task_id);
			confirmation->set_error(false);
			network->send(this, network->controllerGetIdentifier(), Message::WorkerTaskConfirmation, NULL, 0, &p);
			
		}
		*/
		
	};
}

#endif
