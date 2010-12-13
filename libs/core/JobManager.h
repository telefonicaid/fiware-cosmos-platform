#ifndef _H_JOB_MANAGER
#define _H_JOB_MANAGER

#include "Lock.h"							// au::Lock
#include <map>								// std::map
#include <vector>							// std::vector
#include "Endpoint.h"						// ss::Endpoint
#include <sstream>							// ss::ostringstream
#include "samson.pb.h"						// network::Messages...
#include "au_map.h"							// au::map
#include "ControllerTaskManager.h"			// ss::ControllerTaskManager

namespace ss {
	
	class SamsonController;
	class ControllerTask;
	class Job;
	
	/**
	 
	 Job Manager is the top level process managerment element at SamsonController
	 
	 It basically receives commands from delailah to create a job.
	 In some cases, jobs are just a single task (ex add_queue andreu system.UInt system.UInt)
	 In some cases, jobs are a single command like a map/reduce/parse/Etc..
	 In other cases, jobs are scripts that comes with operations and other scritps inside
	 
	 */
	
	class JobManager
	{
		au::map<size_t ,Job> job;				// List of jobs pending to be executed
		au::Lock lock;							// Mutex to protect "job"
		
		SamsonController * controller;		
		ControllerTaskManager taskManager;		// Internal task manager to submit tasks ( a task is a distributd command)
		
		friend class Job;						// To access controller
		friend class ControllerTaskManager;		// To notify about a finish task
		
	public:
		
		JobManager(SamsonController * _controller) : taskManager( this ) 
		{
			controller = _controller;
		}
		
		// Add a job from a delilah command
		void addJob(int fromId , const network::Command &command , size_t sender_id  );
	
		// Notification from the network( workers ) about a particular task
		void notifyWorkerConfirmation( int worker_id , network::WorkerTaskConfirmation* confirmationMessage );
		
		// Fill information of this packet
		void fill(network::JobList *jl , std::string command);
		
	private:

		// Remove a job
		void _removeJob( Job *j );

	
		
	};
}

#endif
	
	