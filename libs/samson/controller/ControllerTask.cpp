

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
		
        // Init the total amount of information to be process
        if( info )
        {
            for ( size_t i = 0 ; i < info->input_queues.size() ; i++)
            {
                network::FullQueue * q = info->input_queues[i]; 
                total_info.append( (size_t)q->queue().info().size() , (size_t)q->queue().info().kvs() );
            }
            
            // Copy environemtn variables from _info
            environment.copyFrom( &_info->environment );
            //environment.set("command" , _info->command );
            
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
        
        
        // Set environment variables of the job
        Environment *env = &info->job->environment;
        network::Environment *e = t->mutable_environment();	
        std::map<std::string,std::string>::iterator iter;
        for ( iter = env->environment.begin() ; iter != env->environment.end() ; iter++)
        {
            network::EnvironmentVariable *ev = e->add_variable();			
            
            ev->set_name( iter->first );
            ev->set_value( iter->second );
        }

        // Set local environments of this task
        for ( iter = environment.environment.begin() ; iter != environment.environment.end() ; iter++)
        {
            network::EnvironmentVariable *ev = e->add_variable();			
            
            ev->set_name( iter->first );
            ev->set_value( iter->second );
        }

        
    }
    
    
    void ControllerTask::getInfo( std::ostringstream& output)
    {
        au::xml_open(output, "controller_task");
        
        au::xml_simple(output , "id" , id );
        au::xml_simple(output , "job_id" , job->getId() );
        
        au::xml_simple(output, "name", info->operation_name );
        
        switch ( state ) {
            case init:
                au::xml_simple(output , "state" , "init" );
                break;
            case running:
                au::xml_simple(output , "state" , "running" );

                au::xml_simple(output , "finished_workers" , "finished_workers" );
                au::xml_simple(output , "num_workers" , "num_workers" );

                au::xml_single_element( output , "total_info" , &total_info );
                au::xml_single_element( output , "running_info" , &running_info );
                au::xml_single_element( output , "processed_info" , &processed_info );

                break;
            case finished:
                au::xml_simple(output , "state" , "finished" );
                break;
            case completed:
            {
                au::xml_simple(output , "state" , "completed" );

                if( error.isActivated() )
                    au::xml_simple(output , "error" , error.getMessage() );
            }
                break;
                
            default:
                break;
        }
        
        au::xml_close(output ,"controller_task");
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
        t->set_worker( workerIdentifier );
        t->set_num_workers( num_workers );
        
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

        