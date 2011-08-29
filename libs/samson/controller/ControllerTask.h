#ifndef _H_CONTROLLER_TASK
#define _H_CONTROLLER_TASK

#include "au/Lock.h"							// au::Lock
#include <map>								// std::map
#include <vector>							// std::vector
#include "samson/network/Endpoint.h"						// samson::Endpoint
#include <sstream>							// std::ostringstream
#include <iostream>							// std::cout
#include "samson/common/samson.pb.h"						// network::...
#include <set>								// std::set
#include "ControllerTaskInfo.h"				// samson::ControllerTaskInfo
#include "samson/common/samson.pb.h"						// samson::network::...
#include "samson/module/Environment.h"				// samson::Environment
#include "Job.h"							// samson::Job
#include "samson/common/coding.h"                         // KVInfo
#include "samson/common/samson.pb.h"                      // network::
#include "au/ErrorManager.h"                       // au::ErrorManager

namespace samson {
	
	class SamsonController;
	class ControllerDataManager;
    class ControllerTaskManager;

	/**
	 Task at the controller
	 Managed by ControllerTaskManager
	 */
	
	class ControllerTask
	{
		friend class ControllerDataManager;
        friend class ControllerTaskManager;
		friend class JobManager;
		friend class Job;

		ControllerTaskInfo *info;               // Information for this task ( extracted from DataManager )
        ControllerTaskManager *taskManager;     // Pointer to the taskManager
        
		Job *job;                               // Pointer to the job we belong
		size_t id;                              // Id of the task ( shared by all the workers )

        std::set<int> workers;                  // Identifiers of all the workers involved in this task
        std::set<int> workers_finished;         // Identifiers of the workers that have finished producing data
        std::set<int> workers_completed;        // Identifiers of all the workers that have completed the operation
        
		int num_workers;                        // Total workers that have to confirm the task
		int finished_workers;                   // List of worker ids that have reported finish
		int complete_workers;                   // List of worker ids that have reported complete	
				
		// Error management
        au::ErrorManager error;

        
        // Information about progress
        FullKVInfo total_info;                  // Total amount of information to be processed
        FullKVInfo running_info;                // Total amount that started to run at some point ( accumulated )
        FullKVInfo processed_info;              // Total amount that has been processed
        
        // Particular environment for this task ) ( Currently only used to inform workers about the original command )
        Environment environment;
        
	public:
		
        enum ControllerTaskState
        {
            init,
            running,
            finished,
            completed        // Complete process with or without error
        };

    private:
        
        ControllerTaskState state;
        
    public:
		
		ControllerTask( size_t _id , Job *_job, ControllerTaskInfo *_info , int _total_workers );
		~ControllerTask();

		// Controller task
        void startTask();
        void notify( int worker_id , network::WorkerTaskConfirmation *confirmation);
        
        // Getting information
		size_t getId();
		size_t getJobId();
        ControllerTaskState getState();
        void getInfo( std::ostringstream& output);
		int getNumUsedOutputs();
		int getNumOutputs();
		void fillInfo( network::WorkerTask *t , int workerIdentifier );		
        
    private:
        
        // Send messages
        void fill( network::ControllerTask* task );

        // Send a message to controller
        void sendWorkerTasks( );
        void sendWorkerTask( int workerIdentifier );


		
		
	};
	

	
}

#endif

	
