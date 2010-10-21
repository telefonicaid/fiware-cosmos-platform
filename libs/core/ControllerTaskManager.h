#ifndef _H_CONTROLLER_TASK_MANAGER
#define _H_CONTROLLER_TASK_MANAGER

#include "Lock.h"							// au::Lock
#include <map>								// std::map
#include <vector>							// std::vector
#include "Endpoint.h"						// ss::Endpoint
#include <sstream>							// ss::ostringstream
#include "samson.pb.h"						// network::Messages...

namespace ss {

	class SamsonController;
	class ControllerTask;
	
	
	/**
	 Manager of running tasks at controller
	 */
	
	class ControllerTaskManager
	{
		au::Lock lock;								// Lock to protect the current list of tasks
		std::map< size_t , ControllerTask*> task;	// Map of tasks currently running
		size_t current_task_id;
		
		SamsonController *controller;
		
	public:
		
		ControllerTaskManager( SamsonController * _controller)
		{
			controller = _controller;
			current_task_id = 1;		// First task is "1" since "0" means error running task
		}

		/**
		 Add a particular task into the controller scheduler
		 */
		
		bool addTask( int fromIdentifier, std::string command , std::ostringstream& output );
		
		/**
		 Noitify a confirmation from workers
		 */
		
		void notifyWorkerConfirmation(int from, network::WorkerTaskConfirmation confirmationMessage );
		
		/**
		 Get status information about tasks
		 */
		
		std::string status();
		
		
	private:
		
		/**
		 Create the task form the command string
		 */
		
		ControllerTask* createTask(int fromIdentifier, std::string command , std::ostringstream& output );

		/**
		 Real implemetation of addTask
		 */
		
		bool _addTask( int fromIdentifier, std::string command , std::ostringstream& output );

		
		ControllerTask * findTask(size_t task_id)
		{
			std::map< size_t , ControllerTask*>::iterator t =  task.find( task_id );
			if( t!= task.end() )
				return NULL;
			else
				return t->second;
		}
		
		
	};

}


#endif
