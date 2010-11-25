#ifndef _H_CONTROLLER_TASK_MANAGER
#define _H_CONTROLLER_TASK_MANAGER

#include "Lock.h"							// au::Lock
#include <map>								// std::map
#include <vector>							// std::vector
#include "Endpoint.h"						// ss::Endpoint
#include <sstream>							// ss::ostringstream
#include "samson.pb.h"						// network::Messages...
#include "au_map.h"							// au::map

namespace ss {

	class SamsonController;
	class ControllerTask;
	class ControllerTaskInfo;
	
	/**
	 Manager of running tasks at controller
	 */
	
	class ControllerTaskManager
	{
		au::Lock lock;								// Lock to protect the current list of tasks
		au::map< size_t , ControllerTask > task;		// Map of tasks currently running

		size_t current_task_id;						// Internal counter to give new task_ids
		
		SamsonController *controller;				// Pointer to the controller to interact with the resto of elements (network included)
		
	public:
		
		ControllerTaskManager( SamsonController * _controller)
		{
			controller = _controller;
			current_task_id = 1;		// First task is "1" since "0" means error running task
		}

		/**
		 Add a particular task into the controller scheduler from delailah command
		 */
		
		size_t addTask( ControllerTaskInfo *info , size_t job_id );

		/**
		 Noitify a confirmation from workers
		 */
		
		void notifyWorkerConfirmation(int from, network::WorkerTaskConfirmation confirmationMessage );
		
		/**
		 Get status information about tasks
		 */
		
		std::string getStatus();
		
		
	private:

		// Send a message to a worker with a particular task
		void sendWorkerTasks( ControllerTask *task );
		void sendWorkerTask( int  workerIdentifier , size_t task_id , ControllerTask *task );
		
		
		
	};

}


#endif
