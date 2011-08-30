
#include "JobManager.h"			// Own interface
#include "Job.h"				// samson::Job
#include "samson/controller/SamsonController.h"
#include "samson/network/Packet.h"				// samson::Packet
#include "ControllerTask.h"		// samson::ControllerTask

namespace samson {
    
	void JobManager::addJob(int fromId, const network::Command &command , size_t sender_id )
	{
		
		// Get the new id for this job from the data manager
		size_t job_id = controller->data.getNewTaskId();
		
		if( fromId != -1 )
		{			
			// Send a message to delilah to confirm this new job
			Packet *p2 = new Packet(Message::CommandResponse);
			network::CommandResponse *response = p2->message->mutable_command_response();
			response->mutable_command()->CopyFrom( command );
			response->set_new_job_id( job_id );
			p2->message->set_delilah_id( sender_id );
			controller->network->send( fromId, p2 );
		}
		
		// Create the job itself
		Job *j = new Job( this , job_id, fromId , command, sender_id );
		
		// Notify to the task manager that this "job" is about to start
		controller->data.beginTask( job_id , command.command() );
		
		// Insert in the map of jobs
		job.insertInMap( job_id , j );
        
		// Run the job until a task is scheduled or error/finish
		j->run();
		
	}
    
    JobManager::~JobManager()
    {
        // Remove pending jobs objects
        job.clearMap();
    }
    
    
	// Kill a particular job
	void JobManager::kill( size_t job_id ,  std::string message  )
	{
        
		Job* j = job.findInMap( job_id );
		if( j )
			j->kill( message );
		
	}
	
    // Kill all current jobs with this error message
    void JobManager::killAll( std::string message )
    {
		au::map<size_t ,Job>::iterator j;
        for (j = job.begin() ; j != job.end() ; j++)
        {
            Job* _job = j->second;
            if( _job->status() == Job::running )
                kill( _job->getId() , message );
        }
        
    }
	
    
    
	void JobManager::notifyWorkerConfirmation( int worker_id , network::WorkerTaskConfirmation* confirmationMessage  )
	{
        
		// Get the task
		size_t task_id			= confirmationMessage->task_id();	// Get the task_id
		ControllerTask *task	= taskManager.getTask( task_id );	// Get the task it refers to		
        
		if( ! task )
            LM_W(("Received a confirmation from a worker %d for a non-existing task  %lu. Ignoring...", worker_id , task_id));
        
		Job* job = NULL;
		if( task )
			job = task->job;
        
        
		// Add of Remove files independently of the type of message
		if( task && job )
		{
			for (int f = 0 ; f < confirmationMessage->add_file_size() ; f++)
			{
				const network::QueueFile& qfile = confirmationMessage->add_file(f);
				const network::File& file = qfile.file();
				const network::KVInfo& info = file.info();
				
				std::string command = ControllerDataManager::getAddFileCommand(file.worker() , file.name(),  info.size() , info.kvs() , qfile.queue() );
				DataManagerCommandResponse response = controller->data.runOperation( job->getId() , command );
				
                
			}
			
			for (int f = 0 ; f < confirmationMessage->remove_file_size() ; f++)
			{
				const network::QueueFile& qfile = confirmationMessage->remove_file(f);
				const network::File& file = qfile.file();
				const network::KVInfo& info = file.info();
				
				std::string command = ControllerDataManager::getRemoveFileCommand(file.worker() , file.name(),  info.size() , info.kvs() , qfile.queue() );
				DataManagerCommandResponse response = controller->data.runOperation( job->getId() , command );
				
			}
			
		}
        
        // Notify this message to the task
        task->notify( worker_id , confirmationMessage );
        
        // Review the task manager to see if it is necessary to run new tasks
        taskManager.reviewTasks();

    }
    
    void JobManager::removeAllFinishJobs()
    {
        
        Job *job = _getNextFinishJob();
        while( job )
        {
            _removeJob(job);
            job = _getNextFinishJob();
        }
        
    }
    
    Job* JobManager::_getNextFinishJob()
    {
        
        std::map<size_t,Job*>::iterator iter;
        for( iter = job.begin() ; iter != job.end() ; iter++)
        {
            Job *job = iter->second;
            if( ( job->status() == Job::error ) || ( job->status() == Job::finish ) )
                return job;
        }
        
        return NULL;
    }	
    
    void JobManager::_removeJob( Job *j )
    {		
        if( ( j->status() != Job::error ) && ( j->status() != Job::finish ) )
            LM_X(1,("Internal error"));
        
        // Remove all tasks associated to this job ( all of them are suppoused to be completed)
        j->removeTasks();
        
        // Remove from the map of jobs
        j = job.extractFromMap( j->getId() );
        
        // Delete the job object itself
        delete j;
        
    }

    void JobManager::getInfo( std::ostringstream& output)
    {
        au::xml_open(output , "job_manager");
        au::xml_iterate_map( output , "jobs" , job );
        au::xml_close(output , "job_manager");
        
        taskManager.getInfo(output);        
    }


}