
#include "JobManager.h"			// Own interface
#include "Job.h"				// ss::Job
#include "SamsonController.h"

namespace ss {

	void JobManager::addJob(int fromIdentifier, int _sender_id  , std::string command )
	{
		lock.lock();
		
		// At the moment only single-line commands are allowed
		Job *j = new Job( controller, current_job_id++ , fromIdentifier , _sender_id, command );
		
		// Insert in the list
		//job.insertInMap( j->getId() , j ); // Why this is not working ??? second time today :(
		job.insert( std::pair<size_t , Job*>( j->getId() , j ) );
		
		j->run();

		// Send a confirmation message if it is not finished
		if( !j->isFinish() )
		{
			std::ostringstream output;
			output << "Job Scheduled with job id " << j->getId() << std::endl;
			controller->sendDelilahAnswer(_sender_id, fromIdentifier, false, false, output.str());
		}
		
		// Just in case there is an error at this level
		purgeJob( j );
		
		lock.unlock();
		
	}

	

	void JobManager::notifyFinishTask( size_t job_id , size_t task_id )
	{
		lock.lock();
		
		Job *j =  job.findInMap( job_id );
		if( j )
		{
			j->notifyTaskFinish( task_id );
			purgeJob( j );
		}
		
		lock.unlock();
		
	}
	
	void JobManager::purgeJob( Job *j)
	{
		// TODO: Consider chained jobs
		if( j->isFinish() )
		{
			// Remove from the map of jobs
			j = job.extractFromMap( j->getId() );
			delete j;
		}
	}
	
}