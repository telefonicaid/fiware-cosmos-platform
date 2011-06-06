

#include "ControllerTask.h"			// Own interface
#include "JobManager.h"				// samson::JobManager
#include "ControllerDataManager.h"	// samson::ControllerDataManager

#include "samson/network/Packet.h"                 // samson::Packet
#include "samson/network/NetworkInterface.h"       // samson::NetworkInterface
#include "samson/controller/SamsonController.h"       // samson::SamsonController


namespace samson {
    
	ControllerTask::ControllerTask( size_t _id , Job *_job, ControllerTaskInfo *_info , int _total_workers )
	{
		// Keep the command and the id
		id = _id;
		
		// Keep a pointer to the job
		job = _job;
		
		// Elements
		info = _info;
        
		// total number of workers to wait for this number of confirmation ( in case we sent to workers )
		finished_workers = 0;
		complete_workers = 0;
		num_workers = _total_workers;
        
        // Initial status 
		state = init;
		
		// In generators, this is used to determine how is the active user
		generator = rand()%num_workers;
        
        // Init the total amount of information to be process
        if( info )
        {
            for ( size_t i = 0 ; i < info->input_queues.size() ; i++)
            {
                network::FullQueue * q = info->input_queues[i]; 
                total_info.append( (size_t)q->queue().info().size() , (size_t)q->queue().info().kvs() );
            }
        }
		
	}
	
	ControllerTask::~ControllerTask()
	{
		delete info;
	}
    
    void ControllerTask::notify( int worker_id , network::WorkerTaskConfirmation *confirmationMessage )
    {
        
        // Update the internal status of this task
        
        if( confirmationMessage->has_progressprocessed() )
            processed_info.append( (size_t)confirmationMessage->progressprocessed().size() , (size_t)confirmationMessage->progressprocessed().kvs() );
        
        if( confirmationMessage->has_progressrunning() )
            running_info.append( (size_t)confirmationMessage->progressrunning().size() , (size_t)confirmationMessage->progressrunning().kvs() );
        
        // Internal check for integrity
        if( state == running )
        {
            if(!job->isCurrentTask(this) )	// we can only receive finish reports from the current task
                LM_X(1,("Internal error"));
        }            
        
        // Depending of the type of message
		switch ( confirmationMessage->type() ) 
		{
			case network::WorkerTaskConfirmation::finish:
			{
                
                if( state == running )
                {
                    
                    finished_workers++;
                    if( finished_workers == num_workers )
                    {
                        state = finished;
                        job->notifyCurrentTaskFinish( );
                    }
                    
                }
				break;
                
			case network::WorkerTaskConfirmation::error:
                {
                    
                    if( state != completed)
                    {
                        // Switch to complete state reporting an error !!
                        state = completed;
                        error.set( confirmationMessage->error_message() );
                        
                        std::ostringstream str_error;
                        str_error << "[W:" << worker_id << "] ";
                        str_error << confirmationMessage->error_message();
                        
                        job->notifyCurrentTaskReportedError( str_error.str() );
                    }
                    
                }
				break;
			case network::WorkerTaskConfirmation::complete:
                {
                    complete_workers++;
                    
                    if( state == finished )
                        if( complete_workers == num_workers )
                            state = completed;
                    
                    // Verify if we are only waiting for writing
                    if( job->status() == Job::saving )
                        job->run();
                    
                }
				break;
			default:
                {
                    // Nothing in particular here
                }
                break;
                
            }
        }
    }
    
    void ControllerTask::startTask()
    {
        if( state != init )
            LM_X(1,("Internal error"));
        
        // Send this task to all the workers
        state = running;
        
        // Send the controll message to all the workers
        sendWorkerTasks( );										
    }
    
    size_t ControllerTask::getId()
    {
        return id;
    }
    
    size_t ControllerTask::getJobId()
    {
        return job->getId();
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
        
        switch ( state ) {
            case init:
                o << " (init) ";
                break;
            case running:
                o << " running ";
                o << " Workers:" << finished_workers << " / " << complete_workers << " / " << num_workers;
                o << " Progress: " << processed_info.str() << " / " << running_info.str() << " / " << total_info.str(); 
                break;
            case finished:
                o << " finished ";
                break;
            case completed:
            {
                if( error.isActivated() )
                    o << "error (" << error.getMessage() << ")";
                else
                    o << " completed ";
            }
                break;
                
            default:
                break;
        }
        
        return o.str();
    }
    
    void ControllerTask::fill( network::ControllerTask* task )
    {
        switch (state) {
            case init:
                task->set_state( network::ControllerTask_ControllerTaskState_ControllerTaskRunning );
                break;
            case running:
                task->set_state( network::ControllerTask_ControllerTaskState_ControllerTaskRunning );
                break;
            case finished:
                task->set_state( network::ControllerTask_ControllerTaskState_ControllerTaskFinish );
                break;
            case completed:
                task->set_state( network::ControllerTask_ControllerTaskState_ControllerTaskCompleted );
                break;
            default:
                break;
        }
        
        if( error.isActivated() )
            task->mutable_error()->set_message( error.getMessage() );
        
        
        // set job & task id
        task->set_job_id( job->getId() );
        task->set_task_id( id );
        
        // KVInfo for total running and process
        copy( &total_info , task->mutable_total_info() );
        copy( &running_info , task->mutable_running_info() );
        copy( &processed_info , task->mutable_processed_info() );
        
    }
    
    /* ****************************************************************************
     *
     * sendWorkerTasks - 
     */
    void ControllerTask::sendWorkerTasks( )
    {
        // Send messages to the workers indicating the operation to do (waiting the confirmation from all of them)
        
        for (int i = 0 ; i < num_workers ; i++)
            sendWorkerTask( i );
    }	

    
    void ControllerTask::sendRemoveMessageToWorkers()
    {
        for (int i = 0 ; i < num_workers ; i++)
        {
            // Get status of controller
            Packet *p2 = new Packet( Message::WorkerTaskRemove );

            // Set the task id
            network::WorkerTaskRemove *t = p2->message->mutable_worker_task_remove();
            t->set_task_id( id );
            
            taskManager->controller->network->sendToWorker( i ,  p2);
            
        }
        
    }
    
    /* ****************************************************************************
     *
     * sendWorkerTask - 
     */
    
    void ControllerTask::sendWorkerTask( int workerIdentifier )
    {
        // Get status of controller
        Packet *p2 = new Packet( Message::WorkerTask );
        
        network::WorkerTask *t = p2->message->mutable_worker_task();
        
        t->set_servers( num_workers );
        
        // Fill information for this packet ( input / outputs )
        fillInfo( t , workerIdentifier );
        
        // special flag used in generators
        t->set_generator( generator == workerIdentifier );	
        
        taskManager->controller->network->sendToWorker( workerIdentifier, p2 );
    }
    
    
    ControllerTask::ControllerTaskState ControllerTask::getState()
    {
        return state;
    }
    
    int ControllerTask::getNumUsedOutputs()
    {
        if( state == running )
            return getNumOutputs();
        
        return 0;
    }
    
    int ControllerTask::getNumOutputs()
    {
        if( info ) 
            return info->outputs.size();
        
        return 0;
    }
    
    
    
}

        