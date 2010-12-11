
#include "JobManager.h"			// Own interface
#include "Job.h"				// ss::Job
#include "SamsonController.h"
#include "Packet.h"				// ss::Packet

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
		Job *j = new Job( controller , job_id, fromId , command, sender_id );
		
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
	
	void JobManager::fill(network::JobList *jl , std::string command)
	{
		lock.lock();
		
		std::map<size_t,Job*>::iterator iter;
		for( iter = job.begin() ; iter != job.end() ; iter++)
		{
			Job *job = iter->second;
			
			network::Job *j = jl->add_job();
			j->set_id( job->id );
			j->set_main_command( job->mainCommand );
		}
		
		lock.unlock();
	}


	
	
}