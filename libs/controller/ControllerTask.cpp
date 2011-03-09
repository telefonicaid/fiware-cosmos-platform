

#include "ControllerTask.h"			// Own interface
#include "JobManager.h"				// ss::JobManager
#include "ControllerDataManager.h"	// ss::ControllerDataManager

namespace ss {

	ControllerTask::ControllerTask( size_t _id , Job *_job, ControllerTaskInfo *_info , int _total_workers )
	{
		// Keep the command and the id
		id = _id;
		
		// Keep a pointer to the job
		job = _job;
		
		// Elements
		info = _info;
		
		error = false;
		
		// total number of workers to wait for this number of confirmation ( in case we sent to workers )
		finished_workers = 0;
		complete_workers = 0;
		num_workers = _total_workers;
		
		finish = false;
		complete = false;
		
		
		// In generators, this is used to determine how is the active user
		generator = rand()%num_workers;
		
		
	}
	
	ControllerTask::~ControllerTask()
	{
		delete info;
	}
	
	
	size_t ControllerTask::getId()
	{
		return id;
	}
	
	size_t ControllerTask::getJobId()
	{
		return job->getId();
	}	
	
	void ControllerTask::notifyWorkerFinished()
	{
		finished_workers++;

		if( finished_workers == num_workers )
			finish = true;

	}
	
	void ControllerTask::notifyWorkerComplete()
	{
		complete_workers++;
		if( complete_workers == num_workers )
			complete = true;
	}
	
	void ControllerTask::fillInfo( network::WorkerTask *t , int workerIdentifier )
	{
		t->set_operation( info->operation_name );

		
		// Set the input queues
		for (int i = 0 ; i < (int)info->inputs.size() ; i++)
		{
			network::FullQueue *q = t->add_input_queue();
			q->CopyFrom( *info->input_queues[i] ); 
		}
		
		
		// Set the output queues
		for (int i = 0 ; i < (int)info->outputs.size() ; i++)
		{
			network::FullQueue *q = t->add_output_queue();
			q->CopyFrom( *info->output_queues[i] ); 
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
	
	
}

 