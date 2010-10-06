#ifndef SAMSON_TASK_MANAGER_H
#define SAMSON_TASK_MANAGER_H

#include <list>
#include <vector>
#include <cstdlib>
#include <string>
#include <sstream>
#include <assert.h>
#include <iostream>
#include <deque>

#include "Task.h"                /* Task                            */
#include "Lock.h"                /* Lock                            */



/**
 Task manager is the element in charge on mantaining the queu of tasks
 Add / Remove tasks
 Destroy tasks
 */

namespace ss {

    typedef struct
	{
		std::string message;		// Message ( normal or error )
		bool error;					// Flag to indiciate if there was error in the commnad
	} CommandResponse;
	
	void* core_thread_task(void* p);
	
	class TaskManager
	{
		au::Lock lock;					//!< Lock to add / remove elements in the queue os tasks
		au::StopLock stopLock_cores;	//!< StopLock to wait core threads when no more tasks
		
		std::list<Task*> _tasks;		//!< List of tasks pending to be done
		size_t task_counter;			//!< Counter used to assign tasks ids to the new tasks

		
		TaskManager( ) :  stopLock_cores(&lock)
		{	
			// Init the task counter
			task_counter = 1;				// Task "0" is considered global tasks
			
			// Description  for this lock
			lock.description = "Task Manager Lock";
		}	
		
	public:
		
		int num_working_cores;		//!< Thread safe variable used for monitoring

	public:

		/**
		 Singleton implementation
		 */
		static TaskManager *shared();
		
		/**
		 External unique interface to run commands on the platform
		 If the command is run from a Tasks, parent Task should be provided. ( NULL otherwise )
		 */
		
	public:		
		
		CommandResponse runCommand( Task *parentTask , std::string command , bool preferent_task );
		
	private:
		bool _runSynchCommand( size_t taskId ,  std::string command  , std::stringstream &output , bool& error );
		
		
		/**
		 Main function of all workers
		 */
		
	public:		
		void run_worker( int core_id );
		
		/**
		 Debug function to show tasks
		 */
		
		std::string showTasks();
		
		/**
		 Quit function to leave the platform nicely
		 */
		
		void quit()
		{
			bool pending_tasks = true;
			
			while( pending_tasks )
			{
				std::cout << "Waiting pending tasks\n";
				lock.lock();
				pending_tasks = (_tasks.size()!=0 );
				lock.unlock();
				
				sleep(1);
			}
		}
		
		/**
		 Wake up possible sleeping workers
		 */
		
		void wakeUpWorkers()
		{
			lock.wakeUpAllStopLock( &stopLock_cores );
		}		
		
	private:		
		
		/**
		 Interface with the workers
		 */
		
		Task* getNextTask( int core_id );
		void notifyTaskFinish( Task *task  , bool finish );
		
		
		/**
		 Internal functions to manage tasks
		 */
	private:
		size_t _addTask( Task * task , bool preferent_task );
		bool _start_process( Task * task );
		void _stop_process( Task * task ,  bool finished );	
		void _removeTask( Task* task );
		

		
	};
}

#endif
