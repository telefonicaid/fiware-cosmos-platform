#ifndef _H_CONTROLLER_TASK
#define _H_CONTROLLER_TASK

#include "Lock.h"							// au::Lock
#include <map>								// std::map
#include <vector>							// std::vector
#include "Endpoint.h"						// ss::Endpoint
#include <sstream>							// std::ostringstream
#include <iostream>							// std::cout
#include "samson.pb.h"						// network::...
#include <set>								// std::set
#include "ControllerTaskInfo.h"				// ss::ControllerTaskInfo
#include "samson.pb.h"						// ss::network::...
#include "samson/Environment.h"				// ss::Environment
#include "Job.h"							// ss::Job

namespace ss {
	
	class SamsonController;
	class ControllerDataManager;

	/**
	 Task at the controller
	 Managed by ControllerTaskManager
	 */
	
	class ControllerTask
	{
		friend class ControllerDataManager;
		friend class JobManager;

		ControllerTaskInfo *info;		// Information for this task ( extracted from DataManager )
		
		Job *job;						// Pointer to the job we belong
		size_t id;						// Id of the task ( shared by all the workers )

		int num_workers;				// Total workers that have to confirm the task

		std::set<int> finish_workers;	// List of worker ids that have reported finish
		std::set<int> complete_workers;	// List of worker ids that have reported complete	

		
		int generator;					// Spetial flag to be removed from here ;)
		
		int *num_items;					// Total items in this woker
		int *num_finish_items;			// Total items finished in this worker
		int total_num_finish_items;
		int total_num_items;
		
		
		// Error management
		bool error;
		std::string error_message;

		friend class ControllerTaskManager;
		
	public:
		
		bool finish;					// Flag to indicate that the task is finished ( by all workers )
		bool complete;					// Flag to indicate that the task is completed ( by all workers )
		
		ControllerTask( size_t _id , Job *_job, ControllerTaskInfo *_info , int _total_workers );
		
		~ControllerTask();
		
		size_t getId();
		
		size_t getJobId();
		
		void notifyWorkerConfirmation( int worker_id , network::WorkerTaskConfirmation* confirmationMessage , ControllerDataManager * data );
		
		void fillInfo( network::WorkerTask *t , int workerIdentifier );
		
		// Update with the added files
		void updateData( ControllerDataManager * data );
		
		std::string getStatus()
		{
			std::ostringstream o;
			o << "Task " << id << " : " << info->command;
			return o.str();
		}
		
	};
	

	
}

#endif

	
