#include "ControllerTaskManager.h"		// Own interface
#include "ControllerTask.h"				// ss::ControllerTask
#include "SamsonController.h"			// ss::SamsonController
#include "CommandLine.h"				// au::CommandLine
#include "Endpoint.h"					// ss::Endpoint
#include "Packet.h"						// ss::Packet

namespace ss
{

	size_t ControllerTaskManager::addTask( std::string command , size_t job_id  )
	{
		lock.lock();
		
		ControllerTask * t = new ControllerTask( current_task_id++ , job_id,  command , controller->network->getNumWorkers() );
		
		//task.insertInMap( t->getId() , t ); // No idea why this is not working :(
		task.insert( std::pair<size_t , ControllerTask*>( t->getId() , t));
			
		// Put the task in running by sending to all workers
		controller->sendWorkerTasks( t );										// Send the command to all the workers to perform this task
		
		lock.unlock();

		return t->getId();
		
	}
	
	void ControllerTaskManager::notifyWorkerConfirmation( int worker_id, network::WorkerTaskConfirmation confirmationMessage )
	{
		
		// get the task id that is confirmed
		size_t task_id = confirmationMessage.task_id();

		lock.lock();
		
		ControllerTask * t = task.findInMap( task_id );
		
		if( t )
		{
			// Get the job id
			size_t job_id = t->getJobId();
			
			// Notify that this worker has answered
			t->notifyWorkerConfirmation( worker_id , confirmationMessage );
			
			if( t->isFinish() )
			{
				// Notify the JobManager that this task is finish
				controller->jobManager.notifyFinishTask( job_id ,task_id , t->confirmationMessages );
				
				// Delete this task from this manager
				t = task.extractFromMap( task_id );
				delete t;
				
				
			}
		}
		lock.unlock();
	}
	
	
	
	std::string ControllerTaskManager::status()
	{
		std::stringstream o;
		o << "Task Manager:" << std::endl;
		std::map< size_t , ControllerTask*>::iterator t;
		for (t = task.begin() ; t != task.end() ; t++)
			o << t->second->str() << std::endl;
		return o.str();
		
	}
	
	
}
