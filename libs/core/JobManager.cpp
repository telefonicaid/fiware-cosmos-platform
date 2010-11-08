
#include "JobManager.h"			// Own interface
#include "Job.h"				// ss::Job
#include "SamsonController.h"

namespace ss {

	void JobManager::addJob(int fromIdentifier, int _sender_id  , std::string command )
	{
		lock.lock();
		
		// New id
		size_t job_id = current_job_id++;
		
		// At the moment only single-line commands are allowed
		Job *j = new Job( controller , job_id, fromIdentifier , _sender_id, command );
		
		// Notify to the task manager that this "job" is about to start
		controller->data.beginTask( job_id );
		
		// Insert in the list
		job.insert( std::pair<size_t , Job*>( j->getId() , j ) );

		
		// Run the job until a task is scheduled
		j->run();
		
		
		// Send a confirmation message if it is not finished
		if( j->isFinish() )
		{
			if( j->isError() )
				controller->data.cancelTask( job_id );
			else
				controller->data.finishTask( job_id );
			
			j->sentConfirmationToDelilah( );
			removeJob(j);
		}
		
		lock.unlock();
	}
	
	void JobManager::notifyFinishTask( size_t job_id , size_t task_id , std::vector<network::WorkerTaskConfirmation> &confirmationMessages )
	{
		lock.lock();
		
		Job *j =  job.findInMap( job_id );
		if( j )
		{
			j->notifyTaskFinish( task_id , confirmationMessages );
			
			if( !j->isFinish() )
			{
				controller->data.finishTask( job_id );
				j->sentConfirmationToDelilah( );
				removeJob(j);
			}
		}
		
		lock.unlock();
		
	}
	
	void JobManager::removeJob( Job *j )
	{
		assert( j->isFinish() );

		// Remove from the map of jobs
		j = job.extractFromMap( j->getId() );
		delete j;
		
	}
	
	
}