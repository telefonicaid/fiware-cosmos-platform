#include "logMsg/logMsg.h"               // LM_*
#include "logMsg/traceLevels.h"          // Trace Levels

#include "samson/network/iomMsgSend.h"           // iomMsgSend
#include "au/CommandLine.h"                     // au::CommandLine
#include "samson/worker/SamsonWorker.h"         // SamsonWorker
#include "WorkerTaskManager.h"                  // Own interface
#include "samson/network/Packet.h"              // samson::Packet
#include "WorkerTask.h"                         // samson::WorkerTask
#include "samson/common/SamsonSetup.h"          // samson::SamsonSetup
#include "engine/DiskOperation.h"               // samson::DiskOperation
#include "samson/module/ModulesManager.h"       // samson::ModulesManager

namespace samson {
	
	WorkerTaskManager::WorkerTaskManager(SamsonWorker* _worker)
	{
		LM_M(("Created WorkerTaskManager"));
		worker = _worker;

        // Add as a listener for notification_task_finished notifications
        listen( notification_task_finished );

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
			LM_E(("Error in TASK"));
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
            
		   //LM_M(("Received a finish task for task_id %lu with notification %s", task_id , notification->getDescription().c_str() ));
            
            WorkerTask *t = task.findInMap( task_id );
            
            if( t )
            {
                if( t->status == WorkerTask::completed )
                {
                    // Remove the tasks from the task manager
                    // Not removing temporary for testing....
                    // delete task.extractFromMap( task_id );
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
        // Not removing temporary for testing....
		WorkerTask *t = task.findInMap( task_id );

		if( t )
		{
			t->kill();
            
            // Not removing temporary for testing....
			//delete task.extractFromMap(task_id);
		}
	}
	
    void WorkerTaskManager::removeTask( size_t task_id )
    {
		WorkerTask *t = task.extractFromMap( task_id );

        if( t )
        {
            if( t->status != WorkerTask::completed )
                LM_W(("Removing a task that is not completed ( task_id = %lu ). This shoul be an error", task_id));

            delete t;
            
        }
        else
            LM_W(("Trying to remove a non-existing task with id %lu", task_id));
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
