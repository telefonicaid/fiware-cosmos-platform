
#include "JobManager.h"			// Own interface
#include "Job.h"				// ss::Job
#include "SamsonController.h"

namespace ss {

	void JobManager::addJob(int fromId, const network::Command &command , size_t sender_id )
	{
		lock.lock();
		
		// New id
		size_t job_id = controller->data.getNewTaskId();
		
		// At the moment only single-line commands are allowed
		Job *j = new Job( controller , job_id, fromId , command, sender_id );
		
		// Notify to the task manager that this "job" is about to start
		controller->data.beginTask( job_id , command.command() );
		
		// Insert in the list
		job.insert( std::pair<size_t , Job*>( j->getId() , j ) );

		
		// Run the job until a task is scheduled
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
	
	void JobManager::notifyFinishTask( size_t job_id , size_t task_id , bool error, std::string error_message )
	{
		lock.lock();
		
		Job *j =  job.findInMap( job_id );
		if( j )
		{
			j->notifyTaskFinish( task_id , error, error_message );
			
			if( j->isFinish() )
			{
				
				controller->data.finishTask( job_id );
				j->sentConfirmationToDelilah( );
				_removeJob(j);
			}
		}
		
		lock.unlock();
		//std::cout << "End of notify finish task: J:" << job_id << " T: " << task_id << std::endl;
		
	}
	
	void JobManager::_removeJob( Job *j )
	{
		assert( j->isFinish() );

		// Remove from the map of jobs
		j = job.extractFromMap( j->getId() );
		delete j;
		
	}
	
	std::string JobManager::getStatus()
	{
		/*
		std::ostringstream output;

		lock.lock();
		output << getStatusFromArray( job );
		lock.unlock();
		 
		return output.str();
		 */
		return "Error";
	}

	
	
}