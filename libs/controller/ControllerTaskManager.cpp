#include "ControllerTaskManager.h"		// Own interface
#include "ControllerTask.h"				// ss::ControllerTask
#include "SamsonController.h"			// ss::SamsonController
#include "au/CommandLine.h"				// au::CommandLine
#include "Endpoint.h"					// ss::Endpoint
#include "Packet.h"						// ss::Packet
#include "samson/Environment.h"			// ss::Environment
#include "Job.h"						// ss::Job
#include "ControllerTask.h"				// ss::ControllerTask
#include "SamsonSetup.h"				// ss::SamsonSetup

namespace ss
{
	
	ControllerTaskManager::ControllerTaskManager( JobManager * _jobManager)
	{
		jobManager = _jobManager;
		current_task_id = 1;		// First task is "1" since "0" means error running task
		
		
	}
    
    ControllerTaskManager::~ControllerTaskManager()
    {
        // Remove objects of pending tasks
        task.clearMap();
    }

	ControllerTask* ControllerTaskManager::addTask( ControllerTaskInfo *info ,Job *job )
	{
		int num_workers = jobManager->controller->network->getNumWorkers();
		
		ControllerTask * t = new ControllerTask( current_task_id++ , job,  info , num_workers );

		// Stak into internal map
		size_t id = t->getId();
		task.insertInMap( id , t ); 

		reviewTasks();

		return t;
		
	}
	
	ControllerTask* ControllerTaskManager::getTask( size_t task_id )
	{
		return task.findInMap( task_id );
	}
	
	void ControllerTaskManager::removeTask( size_t task_id )
	{
		ControllerTask * t = task.extractFromMap( task_id );

		if(t)
			delete t;
	}
	
	
	void ControllerTaskManager::reviewTasks()
	{
		int max_num_paralell_outputs =  SamsonSetup::shared()->num_paralell_outputs;
		int num_paralell_outputs = 0;

		// Count the number of paralel outputs used in active operations over the cluster
		for ( au::map< size_t , ControllerTask >::iterator t =  task.begin() ; t != task.end() ; t++ )
			num_paralell_outputs += t->second->getNumUsedOutputs();
		
		if( num_paralell_outputs < max_num_paralell_outputs )
		{
			for ( au::map< size_t , ControllerTask >::iterator t =  task.begin() ; t != task.end() ; t++ )
			{
				if( !t->second->running )
				{
					int num_outputs = t->second->getNumOutputs();
					
					if( num_outputs <= (max_num_paralell_outputs - num_paralell_outputs ) )
					{
						// Send this task to all the workers
						t->second->running = true;
						sendWorkerTasks( t->second );										
						
						num_paralell_outputs += num_outputs;
					}
				}
		
			}
			
		}
		
		
		
	};
	
	
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
		Packet *p2 = new Packet( Message::WorkerTask );
		
		network::WorkerTask *t = p2->message->mutable_worker_task();
		
		t->set_servers( jobManager->controller->network->getNumWorkers() );
		
		// Fill information for this packet ( input / outputs )
		task->fillInfo( t , workerIdentifier );
		
		// special flag used in generators
		t->set_generator( task->generator == workerIdentifier );	
		
		NetworkInterface *network = jobManager->controller->network;
		network->send(jobManager->controller,  network->workerGetIdentifier(workerIdentifier) ,  p2);
	}
	    
    
	
	std::string ControllerTaskManager::getStatus()
	{
		std::ostringstream output;
		std::map< size_t , ControllerTask* >::iterator iter;
		for( iter = task.begin() ; iter != task.end() ; iter++)
			output << "\n\t\t\t[" << iter->second->getStatus() << "]";
		return output.str();
	}
	
    void ControllerTaskManager::fill( network::ControllerTaskManagerStatus * status )
    {
		std::map< size_t , ControllerTask* >::iterator iter;
		for( iter = task.begin() ; iter != task.end() ; iter++)
			iter->second->fill( status->add_task() );
        
    }
	
}
