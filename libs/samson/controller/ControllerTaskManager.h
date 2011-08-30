#ifndef _H_CONTROLLER_TASK_MANAGER
#define _H_CONTROLLER_TASK_MANAGER

#include "au/Lock.h"							// au::Lock
#include <map>								// std::map
#include <vector>							// std::vector
#include "samson/network/Endpoint.h"						// samson::Endpoint
#include <sstream>							// samson::ostringstream
#include "samson/common/samson.pb.h"						// network::Messages...
#include "au/map.h"							// au::map

namespace samson {

	class Job;
	class JobManager;
	class ControllerTask;
	class ControllerTaskInfo;
	class SamsonController;
    
	/**
	 Manager of running tasks at controller
	 */
	
	class ControllerTaskManager
	{

		size_t current_task_id;							// Internal counter to give new task_ids
		au::map< size_t , ControllerTask > task;		// Map of tasks currently running

		JobManager *jobManager;
		
        SamsonController *controller;

        friend class ControllerTask;
        friend class JobManager;
        
	public:
		
        
		ControllerTaskManager( JobManager * _jobManager );

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
		
		
        // Get information for monitorization
        void getInfo( std::ostringstream& output);

		
	};

}


#endif
