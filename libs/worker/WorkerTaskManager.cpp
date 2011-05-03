#include "logMsg.h"               // LM_*
#include "traceLevels.h"          // Trace Levels

#include "iomMsgSend.h"           // iomMsgSend
#include "au/CommandLine.h"          // au::CommandLine
#include "SamsonWorker.h"         // SamsonWorker
#include "WorkerTaskManager.h"    // Own interface
#include "Packet.h"               // ss::Packet

#include "WorkerTask.h"           // ss::WorkerTask
#include "SamsonSetup.h"          // ss::SamsonSetup
#include "engine/DiskOperation.h"			// ss::DiskOperation


namespace ss {
	
	WorkerTaskManager::WorkerTaskManager(SamsonWorker* _worker)
	{
		worker = _worker;

        // Add as a listener for notification_task_finished notifications
        engine::Engine::add( notification_task_finished , this );

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
			LM_TODO(("Notify the controller than this task has an error"));
			return;
		}
		else
		{
		
			// Id of this operations
			size_t task_id = worker_task.task_id();
			
			// Create the task
			WorkerTask *t = task.findInMap( task_id );
			if( !t )
			{
				t = new WorkerTask( this );
				task.insertInMap( task_id , t );
			}
            
			// Setup the operation with all the information comming from controller
			t->setupAndRun( op->getType() , worker_task );
			
		}
		
	}
	
    // Notification from the engine about finished tasks
    void WorkerTaskManager::notify( engine::Notification* notification )
    {
        if( !notification->isName(notification_task_finished) )
        {
            LM_W(("WorkerTaskManager received a non correct notification"));
            return;
        }
        
        // Generic parameters of the message
        size_t task_id = notification->environment.getSizeT("task_id", 0);
        
        if( notification->isName( notification_task_finished ) )
        {
            // Find the task to be removed
            
		   LM_M(("Received a finish task for task_id %lu with notification %s", task_id , notification->getDescription().c_str() ));
            
            WorkerTask *t = task.findInMap( task_id );
            
            if( t )
            {
                if( t->status == WorkerTask::completed )
                {
                    // Remove the tasks from the task manager
                    delete task.extractFromMap( task_id );
                }
                else
                    LM_X(1,("WorkerTaskManager received a notification about a finished task but it is not"));
            }
        }
        else
            LM_W(("Unexpected notification at WorkerTaskManager %s", notification->getDescription().c_str() ));
        
    }
    
    bool WorkerTaskManager::acceptNotification( engine::Notification* notification )
    {
        // Only accept notifications for my worker. This is only necessary when testing samsonLocal with multiple workers
        if( notification->environment.getInt("worker", -1) != worker->network->getWorkerId() )
            return false;
        
        return true;
        
    }    
    
	void WorkerTaskManager::killTask( const network::WorkerTaskKill &task_kill )
	{
        
        size_t task_id = task_kill.task_id();
        
        LM_W(("Kill task received for task_id %l", task_id));
        
		// Create the task
		WorkerTask *t = task.extractFromMap( task_id );

		if( t )
		{
			t->kill();
			delete t;
		}
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
		if( t )
        {
			t->finishWorker( worker_from );
        }
        else
		   LM_E(("Finish worker message received for a non-existing task %lu", task_id ));

	}

	void WorkerTaskManager::fill(network::WorkerStatus*  ws)
	{
		ws->set_task_manager_status( getStatus() );
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
