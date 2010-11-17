#include "ControllerTaskManager.h"		// Own interface
#include "ControllerTask.h"				// ss::ControllerTask
#include "SamsonController.h"			// ss::SamsonController
#include "CommandLine.h"				// au::CommandLine
#include "Endpoint.h"					// ss::Endpoint
#include "Packet.h"						// ss::Packet
#include "ObjectWithStatus.h"			// ss::getStatusFromArray(.)
namespace ss
{

	size_t ControllerTaskManager::addTask( std::string command , size_t job_id  )
	{
		lock.lock();
		
		ControllerTask * t = new ControllerTask( current_task_id++ , job_id,  command , controller->network->getNumWorkers() );
		
		//task.insertInMap( t->getId() , t ); // No idea why this is not working :(
		task.insert( std::pair<size_t , ControllerTask*>( t->getId() , t));
			
		// Send this task to all the workers
		controller->sendWorkerTasks( t );										
		
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
	
	
	
	std::string ControllerTaskManager::getStatus()
	{
		std::stringstream o;
		o << getStatusFromArray( task );
		return o.str();
		
	}
	
	
}
