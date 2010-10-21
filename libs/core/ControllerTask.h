#ifndef _H_CONTROLLER_TASK
#define _H_CONTROLLER_TASK

#include "Lock.h"							// au::Lock
#include <map>								// std::map
#include <vector>							// std::vector
#include "Endpoint.h"						// ss::Endpoint
#include <sstream>							// std::ostringstream
#include <iostream>							// std::cout
#include "samson.pb.h"						// network::...

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
			definition,				// We still have not considered what to do with this task
//			queued,					// Waiting to something to happen ( previous task , bloques queue, etc... )
//			ready,					// Ready to send messages to workers
			running,				// Pending workers to send confirmation
//			waiting,				// Waiting some "children" tasks to finish to commit as finised
			finished				// Task is finished
		};
		
		// Identifier of the controller-task ( this has to be aproved in block )
		size_t parent_id;
		size_t id;
		
		// Main command line
		std::string command;
		
		// Status of this tasks
		ControllerTaskStatus status;
		
		int confirmed_workers;	// Number of workers that has confirmed current command
		int total_workers;	
		
		// Information show sent this command ( to notify when finished )
		int fromIdentifier;
		
	public:
		
		ControllerTask( int _fromIdentifier , size_t _parent_id ,  size_t _id , std::string _command , int _total_workers )
		{
			// Keep ids
			id = _id;
			parent_id = _parent_id;
			
			// Keep information about who ordered this
			fromIdentifier = _fromIdentifier;

			// Keep the command to run
			command = _command;

			// total number of workers to wait for this number of confirmation ( in case we sent to workers )
			total_workers = _total_workers;
			
			// Default status
			status = definition;
			
			// Put to zero the counters of workers that has confirmed the tasks
			confirmed_workers = 0;		
			
		}
		
		size_t getId()
		{
			return id;
		}
		
		void notifyWorkerConfirmation( int worker_id , network::WorkerTaskConfirmation confirmationMessage )
		{
			confirmed_workers++;
			
			// If we have received all the notifications from the workers, let's finish
			if( confirmed_workers == total_workers )
				status = finished;
		}
		
		ControllerTaskStatus getStatus()
		{
			return status;
		}
		
		void setRunning()
		{
			assert( status == definition );
			status = running;
		}
		
		std::string getCommand()
		{
			return command;
		}
		
		std::string str()
		{
			std::ostringstream o;
			o << "Task " << id << " : " << command;
			
			switch (status) {
				case definition: o << "[definition]"; break;
				case running: o << "[runnnig]"; break;
				case finished: o << "[finished]"; break;
			}
			
			return o.str();
		
		}
		
		int getFromIdentifier()
		{
			return fromIdentifier;
		}
		
		bool isFinished()
		{
			return ( status == finished);
		}
		
		bool isTopLevelTask()
		{
			return (parent_id == 0);
		}
		
		
		
	};
}

#endif

	
