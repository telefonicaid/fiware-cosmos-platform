#ifndef _H_CONTROLLER_TASK_MANAGER
#define _H_CONTROLLER_TASK_MANAGER

#include "au/Lock.h"							// au::Lock
#include <map>								// std::map
#include <vector>							// std::vector
#include "Endpoint.h"						// ss::Endpoint
#include <sstream>							// ss::ostringstream
#include "samson.pb.h"						// network::Messages...
#include "au/map.h"							// au::map

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
		
		ControllerTaskManager( JobManager * _jobManager);

        ~ControllerTaskManager();
        
		/**
		 Add a particular task into the controller scheduler from delailah command
		 */
		
		ControllerTask* addTask( ControllerTaskInfo *info , Job *job );

		/**
		 Get a particular task
		 */
		
		ControllerTask* getTask( size_t task_id );
	
		/**
		 Review if it is necessary to start another task
		 Sometimes it is not possible to run multiple tasks concurrently to limit the number of "outputs"
		 If to much outputs are active, buffers at the workers will staturate memory
		 */
		
		void reviewTasks();
		
		/**
		 Remove a task ( because the job it belongs finished )
		 */
		
		void removeTask( size_t task_id );
		
		/**
		 Status string of the Controller Task Manager
		 */
		
		std::string getStatus();
		
	private:

		// Send a message to a worker with a particular task
		void sendWorkerTasks( ControllerTask *task );
		void sendWorkerTask( int  workerIdentifier , ControllerTask *task );
		
		
		
	};

}


#endif
