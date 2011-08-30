

#include <sstream>          // std::otringstream


#include "ControllerTaskManager.h"		// Own interface
#include "ControllerTask.h"				// samson::ControllerTask
#include "samson/controller/SamsonController.h"			// samson::SamsonController
#include "au/CommandLine.h"				// au::CommandLine
#include "samson/network/Endpoint.h"					// samson::Endpoint
#include "samson/network/Packet.h"						// samson::Packet
#include "samson/module/Environment.h"			// samson::Environment
#include "Job.h"						// samson::Job
#include "ControllerTask.h"				// samson::ControllerTask
#include "samson/common/SamsonSetup.h"				// samson::SamsonSetup

namespace samson
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

        // Seting me as TaskManager
        t->taskManager = this;
        
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
        int max_num_paralell_outputs = SamsonSetup::getInt("general.max_parallel_outputs");
        
		int num_paralell_outputs = 0;

		// Count the number of paralel outputs used in active operations over the cluster
		for ( au::map< size_t , ControllerTask >::iterator t =  task.begin() ; t != task.end() ; t++ )
			num_paralell_outputs += t->second->getNumUsedOutputs();
		
		if( num_paralell_outputs < max_num_paralell_outputs )
		{
			for ( au::map< size_t , ControllerTask >::iterator t =  task.begin() ; t != task.end() ; t++ )
			{
				if( t->second->getState() == ControllerTask::init )
				{
                    // Get number of output necessary for this task
					int num_outputs = t->second->getNumOutputs();
					
					if( num_outputs <= (max_num_paralell_outputs - num_paralell_outputs ) )
					{
                        t->second->startTask();
						
						num_paralell_outputs += num_outputs;
					}
				}
		
			}
			
		}
	};
	
    // Get information for monitorization
    void ControllerTaskManager::getInfo( std::ostringstream& output)
	{
        au::xml_open(output , "controller_task_manager");

        au::xml_iterate_map( output , "controller_tasks" , task );
        
        au::xml_close( output , "controller_task_manager");
	}
	
}
