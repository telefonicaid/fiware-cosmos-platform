
#include "JobManager.h"			// Own interface
#include "Job.h"				// ss::Job
#include "SamsonController.h"
#include "Packet.h"				// ss::Packet
#include "ControllerTask.h"		// ss::ControllerTask

namespace ss {

	void JobManager::addJob(int fromId, const network::Command &command , size_t sender_id )
	{
		lock.lock();
		
		// New id
		size_t job_id = controller->data.getNewTaskId();

		
		// Send a message to delilah to confirm this new job
		Packet *p2 = new Packet();
		network::CommandResponse *response = p2->message.mutable_command_response();
		response->set_command(command.command());
		response->set_new_job_id( job_id );
		p2->message.set_delilah_id( sender_id );
		controller->network->send(controller, fromId, Message::CommandResponse, p2);
		
		// Create the job itself
		Job *j = new Job( this , job_id, fromId , command, sender_id );
		
		// Notify to the task manager that this "job" is about to start
		controller->data.beginTask( job_id , command.command() );
		
		// Insert in the map of jobs
		job.insertInMap( job_id , j );

		// Run the job until a task is scheduled or error/finish
		j->run();
		
		// Send a confirmation message if it is finished
		if( j->isFinish() )
		{
			if( j->isError() )
				controller->data.cancelTask( job_id , j->getErrorLine() );
			else
				controller->data.finishTask( job_id );
			
			j->sentConfirmationToDelilah( );
			_removeJob(j);
		}
		
		lock.unlock();
		
	}
	
	void JobManager::_removeJob( Job *j )
	{
		assert( j->isFinish() );

		// Remove all tasks associated to this job ( all of them are suppoused to be completed)
		j->removeTasks();
		
		// Remove from the map of jobs
		j = job.extractFromMap( j->getId() );
		
		// Delete the job object itself
		delete j;
		
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

	void JobManager::notifyWorkerConfirmation( int worker_id , network::WorkerTaskConfirmation* confirmationMessage  )
	{
		lock.lock();

		// Get the task
		size_t task_id = confirmationMessage->task_id();
		ControllerTask *task = taskManager.getTask( task_id );
		
		assert(task);
		
		Job* job = task->job;
		assert( job );


		// Update whatever is needed in the task
		task->notifyWorkerConfirmation( worker_id , confirmationMessage , &controller->data );

		// If this is the task the job is waiting, let's continue the job
		if( job->task_id == task->id )
		{
			if ( task->finish )
			{
				job->notifyCurrentTaskFinish(task->error, task->error_message);
			}
			else
			{
				// Update progress of this task
				job->task_num_items = task->total_num_items;
				job->task_num_finish_items = task->total_num_finish_items;
			}
		}
		else
		{
			// make sure we do not report files if we are not the current task of this job
			assert( confirmationMessage->file_size() == 0);
		}
		
		
		if( job->isFinish() )
		{
			
			if(job->isError() )
			{
				controller->data.finishTask( job->id );
				job->sentConfirmationToDelilah( );
				_removeJob(job);	// Remove job and associated tasks
			}
			else if( job->allTasksFinished() )
			{
				controller->data.finishTask( job->id );
				job->sentConfirmationToDelilah( );
				_removeJob(job);	// Remove job and associated tasks
			}
			
		}
		
		
			
		
		
		lock.unlock();
	}

	
	
}