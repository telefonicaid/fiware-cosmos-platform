
#include "JobManager.h"			// Own interface
#include "Job.h"				// ss::Job
#include "SamsonController.h"
#include "Packet.h"				// ss::Packet
#include "ControllerTask.h"		// ss::ControllerTask

namespace ss {

	void JobManager::addJob(int fromId, const network::Command &command , size_t sender_id )
	{
		lock.lock();
		
		// Get the new id for this job from the data manager
		size_t job_id = controller->data.getNewTaskId();
		
		if( fromId != -1 )
		{			
			// Send a message to delilah to confirm this new job
			Packet *p2 = new Packet();
			network::CommandResponse *response = p2->message.mutable_command_response();
			response->set_command(command.command());
			response->set_new_job_id( job_id );
			p2->message.set_delilah_id( sender_id );
			controller->network->send(controller, fromId, Message::CommandResponse, p2);
		}
		
		// Create the job itself
		Job *j = new Job( this , job_id, fromId , command, sender_id );
		
		// Notify to the task manager that this "job" is about to start
		controller->data.beginTask( job_id , command.command() );
		
		// Insert in the map of jobs
		job.insertInMap( job_id , j );

		// Run the job until a task is scheduled or error/finish
		j->run();
		
		lock.unlock();
		
	}

	// Kill a particular job
	void JobManager::kill( size_t job_id )
	{
		lock.lock();

		Job* j = job.findInMap( job_id );
		if( j )
			j->kill();
		
		lock.unlock();
	}
	
	
	void JobManager::notifyWorkerConfirmation( int worker_id , network::WorkerTaskConfirmation* confirmationMessage  )
	{
		lock.lock();

		// Get the task
		size_t task_id = confirmationMessage->task_id();
		ControllerTask *task = taskManager.getTask( task_id );
		Job* job = NULL;
		if( task )
			job = task->job;
		
		
		if( task && job )
		{
			for (int f = 0 ; f < confirmationMessage->add_file_size() ; f++)
			{
				const network::QueueFile& qfile = confirmationMessage->add_file(f);
				const network::File& file = qfile.file();
				const network::KVInfo& info = file.info();
				
				std::string command = ControllerDataManager::getAddFileCommand(file.worker() , file.name(),  info.size() , info.kvs() , qfile.queue() );
				DataManagerCommandResponse response = controller->data.runOperation( job->getId() , command );
				
				// Since this is an internally generated command, it can not have any error of format
				assert( !response.error );
			}
			
			for (int f = 0 ; f < confirmationMessage->remove_file_size() ; f++)
			{
				const network::QueueFile& qfile = confirmationMessage->remove_file(f);
				const network::File& file = qfile.file();
				const network::KVInfo& info = file.info();
				
				std::string command = ControllerDataManager::getRemoveFileCommand(file.worker() , file.name(),  info.size() , info.kvs() , qfile.queue() );
				DataManagerCommandResponse response = controller->data.runOperation( job->getId() , command );
				
				// Since this is an internally generated command, it can not have any error of format
				assert( !response.error );
			}
			
		}
		
		
		
		switch (confirmationMessage->type()) 
		{
				
			case network::WorkerTaskConfirmation::finish:
			{
				//assert(task);
				//assert(job);
				if( task && job )
				{
					assert(job->isCurrentTask(task));	// we can only receive finish reports from the current task
					
					task->notifyWorkerFinished( );		
					if ( task->finish )
						job->notifyCurrentTaskFinish(task->error, task->error_message);
				}

			}
				break;
			case network::WorkerTaskConfirmation::error:
			{
				//assert(task);
				//assert(job);
				if( task && job )
				{
					assert(job->isCurrentTask(task));	// we can only receive finish reports from the current task
					
					//job->setError("Worker", confirmationMessage->error_message());
					task->notifyWorkerFinished( );		
					
					if ( task->finish );
					{
						std::ostringstream str_error;
						str_error << "[W:" << worker_id << "] ";
						str_error << confirmationMessage->error_message();
						job->notifyCurrentTaskFinish(true, str_error.str() );
					}
						
					
				}
				
			}
				break;
			case network::WorkerTaskConfirmation::complete:
			{
				//assert(task);
				//assert(job);
				if( task && job )
				{
				
					if( task )	
						task->notifyWorkerComplete( );		

					// Verify if we are only waiting for writing
					if( job->status() == Job::saving )
						job->run();
				}
				
			}
				break;
			case network::WorkerTaskConfirmation::update:
			{
				// Nothing in particular here
			}
				break;
				
		}
		
		lock.unlock();
	}
	
	void JobManager::fill(network::JobList *jl , std::string command)
	{
		lock.lock();
		
		std::map<size_t,Job*>::iterator iter;
		for( iter = job.begin() ; iter != job.end() ; iter++)
		{
			Job *job = iter->second;
			
			network::Job *j = jl->add_job();
			
			job->fill( j );
		}
		
		lock.unlock();
	}
	
	
	void JobManager::fill( network::ControllerStatus * status )
	{
		lock.lock();
		
		status->set_job_manager_status( _getStatus() );
		status->set_task_manager_status( taskManager.getStatus() );
		
		lock.unlock();
	}
	
	std::string JobManager::_getStatus()
	{
		std::ostringstream output;	
		std::map<size_t ,Job*>::iterator iter;
		
		for (iter = job.begin() ; iter !=job.end() ; iter++)
			output << "[" << iter->second->getStatus() << "]";
		return output.str();
		
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
	
	void JobManager::removeAllFinishJobs()
	{
		lock.lock();

		Job *job = _getNextFinishJob();
		while( job )
		{
			_removeJob(job);
			job = _getNextFinishJob();
		}
		
		lock.unlock();		
	}
	
	
	void JobManager::_removeJob( Job *j )
	{		
		assert( ( j->status() == Job::error ) || ( j->status() == Job::finish ) );
		
		// Remove all tasks associated to this job ( all of them are suppoused to be completed)
		j->removeTasks();
		
		// Remove from the map of jobs
		j = job.extractFromMap( j->getId() );
		
		// Delete the job object itself
		delete j;
		
	}
	
	
	
}