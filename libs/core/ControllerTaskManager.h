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

	class Job;
	class JobManager;
	class ControllerTask;
	class ControllerTaskInfo;
	
	/**
	 Manager of running tasks at controller
	 */
	
	class ControllerTaskManager
	{

		size_t current_task_id;							// Internal counter to give new task_ids
		au::map< size_t , ControllerTask > task;		// Map of tasks currently running

		JobManager *jobManager;
		
	public:
		
		ControllerTaskManager( JobManager * _jobManager)
		{
			jobManager = _jobManager;
			current_task_id = 1;		// First task is "1" since "0" means error running task
		}

		/**
		 Add a particular task into the controller scheduler from delailah command
		 */
		
		size_t addTask( ControllerTaskInfo *info , Job *job );

		/**
		 Get a particular task
		 */
		
		ControllerTask* getTask( size_t task_id );
	
		/**
		 Remove a task ( because the job it belongs finished )
		 */
		
		void removeTask( size_t task_id );
		
	private:

		// Send a message to a worker with a particular task
		void sendWorkerTasks( ControllerTask *task );
		void sendWorkerTask( int  workerIdentifier , ControllerTask *task );
		
		
		
	};

}


#endif
