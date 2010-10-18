#ifndef _H_CONTROLLER_TASK_MANAGER
#define _H_CONTROLLER_TASK_MANAGER

#include "Lock.h"							// au::Lock
#include <map>								// std::map
#include <vector>							// std::vector
#include "Endpoint.h"						// ss::Endpoint

namespace ss {

	class SamsonController;
	
	/**
	 Task at the controller
	 Managed by ControllerTaskManager
	 */
	
	class ControllerTask
	{
		
		enum ControllerTaskStatus {
			definition,				// In definition ( we are still adding command lines to this task )
			ready,					// Ready to send messages to workers
			running,				// Pending workers to send confirmation
			finished				// The last line of this task is completed by all workers
		};
		
		// Identifier of the controller-task
		size_t id;
		
		// Main command line
		std::string main_command;

		// Status of this tasks
		ControllerTaskStatus status;
		
		// Vector of command that need to be executed to finish this top level controller task
		std::vector<std::string> command;
		int command_pos;		// Position inside the vector
		int confirmed_workers;	// Number of workers that has confirmed current command
		int total_workers;	
		
	public:
		
		ControllerTask( size_t id , std::string _main_command , int _total_workers )
		{
			main_command = _main_command;
			status = definition;
			total_workers = _total_workers;
		}
		
		/** 
		 Add individual commands in definition phase
		 */
		
		void addCommand( std::string _command )
		{
			assert( status == definition );
			command.push_back( _command );
		}
		
		/** 
		 Add individual commands in definition phase
		 */
		
		void run()
		{
			assert( status == definition );
			status = ready;
			command_pos = 0;			// Start at the begining of the list of commands
			confirmed_workers = 0;		// Put to zero the counters of workers that has confirmed the tasks
		}

		
		void processCommand()
		{
			assert( status == ready );
			
			if( command_pos == (int)command.size() )
			{
				status = finished;
				return;
			}
			
			status = running;
		}
		
		size_t getId()
		{
			return id;
		}
		
		void notifyWorkerConfirmation( int worker_id )
		{
			confirmed_workers++;
			
			if( confirmed_workers == total_workers )
			{
				confirmed_workers = 0;
				command_pos++;
				
				if( command_pos == (int)command.size() )
					status = finished;
				else
					status = ready;
			}
		}
		
		bool isFinished()
		{
			return (status == finished);
		}
		
		bool isReady()
		{
			return (status == ready);
		}
		
		std::string getCurrentCommand()
		{
			return command[command_pos];
		}
		
		
	};
	
	
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
		
		bool addTask( std::string command , std::ostringstream& output );
		
		/**
		 Noitify a confirmation from workers
		 */
		
		void notifyWorkerConfirmation( size_t task_id , int worker_id );

		
	private:
		
		bool checkAddQueueCommand(std::string command ,  std::ostringstream& output );
		
		
	};

}


#endif