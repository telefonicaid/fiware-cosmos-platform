#include "logMsg/logMsg.h"               // LM_*
#include "logMsg/traceLevels.h"          // Trace Levels

#include "au/CommandLine.h"                     // au::CommandLine

#include "engine/DiskOperation.h"               // engine::DiskOperation
#include "engine/Notification.h"                // engine::Notification

#include "samson/common/SamsonSetup.h"          // samson::SamsonSetup
#include "samson/module/ModulesManager.h"       // samson::ModulesManager

#include "samson/network/iomMsgSend.h"           // iomMsgSend
#include "samson/network/Packet.h"              // samson::Packet

#include "samson/worker/SamsonWorker.h"         // SamsonWorker

#include "WorkerTask.h"                         // samson::WorkerTask

#include "WorkerTaskManager.h"                  // Own interface

#define notification_samson_worker_remove_old_tasks "notification_samson_worker_remove_old_tasks"

namespace samson {
	
	WorkerTaskManager::WorkerTaskManager(SamsonWorker* _worker)
	{
		//LM_M(("Created WorkerTaskManager"));
		worker = _worker;
        
        // Listener to remove all taks
        listen( notification_samson_worker_remove_old_tasks );

        // Run this notification every 5 second
        engine::Notification* notification =  new engine::Notification( notification_samson_worker_remove_old_tasks );
        engine::Engine::shared()->notify( notification , 5 );
        
	}
    
    WorkerTaskManager::~WorkerTaskManager()
    {
        // Remove objects of pending tasks
        task.clearMap();
    }

	
	void WorkerTaskManager::addTask(const network::WorkerTask &worker_task )
	{

		// Look at the operation to 
		Operation *op = ModulesManager::shared()->getOperation( worker_task.operation() );
        
		if( !op )
		{
            LM_M(("Operation %s not found at worker. Ignoring task" , worker_task.operation().c_str() ));
			return;
		}
		else
		{
		
			// Id of this operations
			size_t task_id = worker_task.task_id();
			//LM_M(("Adding task %d", task_id));

			// Create the task
			WorkerTask *t = task.findInMap( task_id );

			if( !t )
			{
				t = new WorkerTask( this );
				task.insertInMap( task_id , t );
			}
            else
				LM_M(("Task %lu: The task has been previously defined by a quick-worker", task_id));

			// Setup the operation with all the information comming from controller
			t->setupAndRun( op->getType() , worker_task );
		}
	}
	
    // Notification from the engine about finished tasks
    void WorkerTaskManager::notify( engine::Notification* notification )
    {
        
        if( notification->isName(notification_samson_worker_remove_old_tasks) )
        {
            std::set<size_t> ids;
            au::map<size_t,WorkerTask>::iterator t;
            for ( t = task.begin() ; t != task.end() ; t++ )
                if( t->second->canBeRemoved() )
                    ids.insert ( t->first );

            for ( std::set<size_t>::iterator t = ids.begin(); t != ids.end() ; t++)
            {
                WorkerTask *_task = task.extractFromMap( *t );
                if( _task )
                {
                    delete _task;
                }
                else
                    LM_W(("Error deleting an old task..."));
            }
            
            return;
        }
        
        
        LM_W(("WorkerTaskManager received an incorrect notification"));
        
    }
        
	void WorkerTaskManager::killTask( const network::WorkerTaskKill &task_kill )
	{
        
        size_t task_id = task_kill.task_id();
        
        //LM_W(("Kill task received for task_id %lu", task_id));
        
		// Create the task
        // Not removing temporary for testing....
		WorkerTask *t = task.findInMap( task_id );

		if( t )
			t->kill( "Kill message received from controller" );
        else
        {
            LM_W(("Task %lu not killed since it is not present in this worker" , task_id));
        }
        
        
        //LM_W(("Killed task for task_id %lu completed", task_id));
        
        
	}
	
	
	void WorkerTaskManager::addBuffer( size_t task_id , network::WorkerDataExchange& workerDataExchange , engine::Buffer* buffer  )
	{
		// Create the task
		WorkerTask *t = task.findInMap( task_id );
		if( !t )
			t = new WorkerTask( this );

		// Add the buffer to the task item
		t->addBuffer( workerDataExchange , buffer );		
	}
	
	void WorkerTaskManager::finishWorker( int worker_from , size_t task_id )
	{
		WorkerTask *t = task.findInMap( task_id );
		if( !t )
        {
            LM_W(("Finish worker message received for a non-existing task %lu. This is possibly a quick-worker", task_id ));
            t = new WorkerTask( this );
			task.insertInMap( task_id , t );
		}        

        t->finishWorker( worker_from );
	}

	
	// Check if a particular task is still active
	bool WorkerTaskManager::checkTask( size_t task_id )
	{
		return ( task.findInMap( task_id ) != NULL);
	}
	
	std::string WorkerTaskManager::getStatus()
	{
		
		std::ostringstream output;
		
		std::map<size_t,WorkerTask*>::iterator iter;
		
		output << "\n";
        
        
		for ( iter = task.begin() ; iter != task.end() ; iter++)
			output << iter->second->getStatus() << "\n";
		
		return output.str();
	}
	
	
}
