

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
		
		// Flag to indicate that we are not running
		running = false;
        
        // Init the total amount of information to be process
        if( info )
        {
            for ( size_t i = 0 ; i < info->input_queues.size() ; i++)
            {
                network::FullQueue * q = info->input_queues[i]; 
                total_info.append( q->queue().info().size() , q->queue().info().kvs() );
            }
        }
		
	}
	
	ControllerTask::~ControllerTask()
	{
		delete info;
	}
	
    void ControllerTask::update( network::WorkerTaskConfirmation* confirmationMessage )
    {
        // Update the internal state of this task
        
        if( confirmationMessage->has_progressprocessed() )
            processed_info.append( confirmationMessage->progressprocessed().size() , confirmationMessage->progressprocessed().kvs() );

        if( confirmationMessage->has_progressrunning() )
            running_info.append( confirmationMessage->progressrunning().size() , confirmationMessage->progressrunning().kvs() );
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
	
	// Auxiliar function to copy a FullQueue including only files for a particular worker_id
	void copy( network::FullQueue * from_queue , network::FullQueue * to_queue , int worker_id )
	{
		to_queue->mutable_queue()->CopyFrom( from_queue->queue() );
		for ( int i = 0 ; i < from_queue->file_size() ; i++)
		{
			if( from_queue->file(i).worker() == worker_id )
				to_queue->add_file()->CopyFrom( from_queue->file(i) );
		}
	}
	
	void ControllerTask::fillInfo( network::WorkerTask *t , int workerIdentifier )
	{
        // Identifier of the job and task ( job is always incremental , task starts from 0 any time platform is restarted )
        t->set_job_id( job->getId() );
		t->set_task_id( id );

        // Set the operation name
		t->set_operation( info->operation_name );

		
		// Set the input queues
		for (int i = 0 ; i < (int)info->inputs.size() ; i++)
		{
			network::FullQueue *q = t->add_input_queue();
			copy( info->input_queues[i] , q , workerIdentifier );
		}
		
		
		// Set the output queues
		for (int i = 0 ; i < (int)info->outputs.size() ; i++)
		{
			network::FullQueue *q = t->add_output_queue();
			copy( info->output_queues[i] , q , workerIdentifier );
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
	
    
    std::string ControllerTask::getStatus()
    {
        std::ostringstream o;
        o << id << " (job: " << job->getId() << ") ";
        
        if( error )
            o << "error";
        else
        {
            if ( complete )
                o << " completed";
            else
                if ( finish )
                    o << " finished";
                    else if( running )
                    {
                        o << " Workers:" << finished_workers << " / " << complete_workers << " / " << num_workers;
                        o << " Progress: " << processed_info.str() << " / " << running_info.str() << " / " << total_info.str(); 
                    }
        }
        
        return o.str();
    }

    void ControllerTask::fill( network::ControllerTask* task )
    {
        if( error )
            task->set_status( network::ControllerTask_ControllerTaskStatus_ControllerTaskError );
        else if ( complete )
            task->set_status( network::ControllerTask_ControllerTaskStatus_ControllerTaskCompleted );
        else if ( finish )
            task->set_status( network::ControllerTask_ControllerTaskStatus_ControllerTaskFinish );
        else
            task->set_status( network::ControllerTask_ControllerTaskStatus_ControllerTaskRunning );
            
        
        task->set_job_id( job->getId() );
        task->set_task_id( id );

        // KVInfo for total running and process
        copy( &total_info , task->mutable_total_info() );
        copy( &running_info , task->mutable_running_info() );
        copy( &processed_info , task->mutable_processed_info() );
        
    }

	
}

 