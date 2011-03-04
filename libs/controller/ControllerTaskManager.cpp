#include "ControllerTaskManager.h"		// Own interface
#include "ControllerTask.h"				// ss::ControllerTask
#include "SamsonController.h"			// ss::SamsonController
#include "CommandLine.h"				// au::CommandLine
#include "Endpoint.h"					// ss::Endpoint
#include "Packet.h"						// ss::Packet
#include "samson/Environment.h"			// ss::Environment
#include "Job.h"						// ss::Job
#include "ControllerTask.h"				// ss::ControllerTask

namespace ss
{

	ControllerTask* ControllerTaskManager::addTask( ControllerTaskInfo *info ,Job *job )
	{
		int num_workers = jobManager->controller->network->getNumWorkers();
		
		ControllerTask * t = new ControllerTask( current_task_id++ , job,  info , num_workers );

		// Stak into internal map
		size_t id = t->getId();
		task.insertInMap( id , t ); 
		
		// Send this task to all the workers
		sendWorkerTasks( t );										

		return t;
		
	}
	
	ControllerTask* ControllerTaskManager::getTask( size_t task_id )
	{
		return task.findInMap( task_id );
	}
	
	void ControllerTaskManager::removeTask( size_t task_id )
	{
		ControllerTask * t = task.extractFromMap( task_id );
		assert( t );
		delete t;
	}
	
	
	/* ****************************************************************************
	 *
	 * sendWorkerTasks - 
	 */
	void ControllerTaskManager::sendWorkerTasks( ControllerTask *task )
	{
		// Send messages to the workers indicating the operation to do (waiting the confirmation from all of them)
		
		for (int i = 0 ; i < jobManager->controller->network->getNumWorkers() ; i++)
			sendWorkerTask(i, task);
	}	
	
	
	
	/* ****************************************************************************
	 *
	 * sendWorkerTask - 
	 */
	void ControllerTaskManager::sendWorkerTask(int workerIdentifier, ControllerTask *task  )
	{
		// Get status of controller
		Packet *p2 = new Packet();
		
		network::WorkerTask *t = p2->message.mutable_worker_task();
		t->set_task_id( task->id );
		
		t->set_servers( jobManager->controller->network->getNumWorkers() );
		
		// Fill information for this packet ( input / outputs )
		task->fillInfo( t , workerIdentifier );
		
		// special flag used in generators
		t->set_generator( task->generator == workerIdentifier );	
		
		NetworkInterface *network = jobManager->controller->network;
		network->send(jobManager->controller,  network->workerGetIdentifier(workerIdentifier) , Message::WorkerTask,  p2);
	}
	
	
	std::string ControllerTaskManager::getStatus()
	{
		std::ostringstream output;
		std::map< size_t , ControllerTask* >::iterator iter;
		for( iter = task.begin() ; iter != task.end() ; iter++)
			output << "[" << iter->second->getStatus() << "]";
		return output.str();
	}
	
	
	
}
