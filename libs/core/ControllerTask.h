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
		
		size_t id;			// Id of the task ( shared by all the workers )
		size_t job_id;		// Id of the job it belongs
				
		ControllerTaskInfo *info;

		int num_completed_workers;		// Total workers that are "completed"
		int num_workers;			// Total workers that have to confirm the task
		
		int generator;
		
		friend class ControllerTaskManager;
		
		bool error;
		std::string error_message;
		
	public:
		
		// Children controller tasks
		ControllerTask( size_t _id , size_t _job_id, ControllerTaskInfo *_info , int _total_workers )
		{
			// Keep the command and the id
			id = _id;
			job_id =_job_id;

			// Elements
			info = _info;
			
			error = false;
			
			// total number of workers to wait for this number of confirmation ( in case we sent to workers )
			num_workers = _total_workers;
			num_completed_workers = 0;

			// In generators, this is used to determine how is the active user
			generator = rand()%num_workers;
		}
		
		~ControllerTask()
		{
			delete info;
		}
		
		size_t getId()
		{
			return id;
		}
		
		size_t getJobId()
		{
			return job_id;
		}
		
		void notifyWorkerConfirmation( int worker_id , network::WorkerTaskConfirmation* confirmationMessage , ControllerDataManager * data );
		
		void fillInfo( network::WorkerTask *t , int workerIdentifier )
		{
			t->set_operation( info->operation_name );
			
			// Set input files
			for (size_t f = 0 ; f < info->input_files.size() ; f++)
			{
				network::FileList all_fl = info->input_files[f];
				network::FileList *fl = t->add_input();

				// Add only files that are placed at that worker
				for (int i = 0 ; i < all_fl.file_size() ; i++)
				{
					if( all_fl.file(i).worker() == workerIdentifier)
						fl->add_file()->CopyFrom( all_fl.file(i) );
				}
				
				//fl->CopyFrom();
			}
			
			
			// Set the output queues
			for (int i = 0 ; i < (int)info->outputs.size() ; i++)
			{
				network::Queue *q = t->add_output();
				network::Queue qq = info->output_queues[i]; 
				q->CopyFrom( qq );
			}
			
			
			// Set environment variables
			Environment *env = &info->job->environment;

			network::Environment *e = t->mutable_environment();	
			std::map<std::string,std::string>::iterator iter;
			for ( iter = env->environment.begin() ; iter != env->environment.end() ; iter++)
			{
				network::EnvironmentVariable *ev = e->add_variable();			
				
				ev->set_name( iter->first );
				ev->set_value( iter->second );
			}
			
		}
		
		// Update with the added files
		void updateData( ControllerDataManager * data );

		
		std::string getStatus()
		{
			std::ostringstream o;
			o << "Task " << id << " : " << info->command;
			return o.str();
		}
		
		
		bool isFinish()
		{
			return ( num_completed_workers ==  num_workers );
		}
		
	};
	

	
}

#endif

	
