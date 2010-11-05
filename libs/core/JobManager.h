#ifndef _H_JOB_MANAGER
#define _H_JOB_MANAGER

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
	class Job;
	
	/**
	 Job Manager is the top level process managerment element at SamsonController
	 It basically receives commands from delailah, creates a job for that an run necessary tasks to finish the job
	 In some cases, jobs are just a single task (ex add_queue andreu system.UInt system.UInt)
	 In some cases a job is a list of tasks and other jobs
	 Before a job can start, it should block all necessary top_level queues
	 */
	
	class JobManager
	{
		au::map<size_t ,Job> job;				// List of jobs pending to be executed
		
		au::Lock lock;							// Mutex to protect "job"
		
		SamsonController * controller;		
		
		size_t current_job_id;
		
	public:
		
		JobManager(SamsonController * _controller) 
		{
			current_job_id = 0;
			controller = _controller;
		}
		
		// Add a job from delilah
		void addJob(int fromId, int _sender_id  , std::string command );
		
		// Notification from the taskManager that a task has finished
		void notifyFinishTask( size_t job_id , size_t task_id );		
		
	private:
		void purgeJob( Job *j);

	
		
	};
}

#endif
	
	