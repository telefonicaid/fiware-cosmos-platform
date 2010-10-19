#ifndef _H_CONTROLLER_TASK
#define _H_CONTROLLER_TASK

#include "Lock.h"							// au::Lock
#include <map>								// std::map
#include <vector>							// std::vector
#include "Endpoint.h"						// ss::Endpoint
#include <sstream>							// std::ostringstream
#include <iostream>							// std::cout

namespace ss {
	
	class SamsonController;
	
	/**
	 Task at the controller
	 Managed by ControllerTaskManager
	 */
	
	class ControllerTask
	{
		friend class ControllerDataManager;
		
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
		
		
		// Information about show ordered this command
		int fromIdentifier;
		
	public:
		
		ControllerTask( int _fromIdentifier , size_t id , std::string _main_command , int _total_workers )
		{
			// Keep information about who ordered this
			fromIdentifier = _fromIdentifier;
			
			main_command = _main_command;
			total_workers = _total_workers;
			
			status = definition;
			command_pos = 0;
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
		
		std::string str()
		{
			std::ostringstream o;
			o << "Task " << id << " : " << main_command;
			o << "(" <<	command_pos <<  "/" << command.size() << ")";
			
			switch (status) {
				case definition: o << "[definition]"; break;
				case ready: o << "[ready]"; break;
				case running: o << "[runnnig]"; break;
				case finished: o << "[finished]"; break;
			}
			
			return o.str();
		
		}
		
		int getFromIdentifier()
		{
			return fromIdentifier;
		}
		
		
	};
}

#endif

	