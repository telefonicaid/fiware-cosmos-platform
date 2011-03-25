#include "logMsg.h"               // LM_*
#include "traceLevels.h"          // Trace Levels

#include "iomMsgSend.h"           // iomMsgSend
#include "CommandLine.h"          // au::CommandLine
#include "SamsonWorker.h"         // SamsonWorker
#include "WorkerTaskManager.h"    // Own interface
#include "Packet.h"               // ss::Packet

#include "WorkerTask.h"           // ss::WorkerTask
#include "SamsonSetup.h"          // ss::SamsonSetup
#include "DiskOperation.h"			// ss::DiskOperation


namespace ss {
	
	WorkerTaskManager::WorkerTaskManager(SamsonWorker* _worker)
	{
		worker = _worker;
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
			t->setup( op->getType() , worker_task );
			
			// Run the operation
			t->run();
			
		}
		
	}
	
	void WorkerTaskManager::killTask( const network::WorkerTaskKill &task_kill )
	{
		// Create the task
		WorkerTask *t = task.extractFromMap( task_kill.task_id() );

		if( t )
		{
			t->kill();
			delete t;
		}
	}
	
	
	void WorkerTaskManager::addBuffer( size_t task_id , network::WorkerDataExchange& workerDataExchange , Buffer* buffer  )
	{
		// Create the task
		WorkerTask *t = task.findInMap( task_id );
		if( !t )
			t = new WorkerTask( this );

		// Add the buffer to the task item
		t->addBuffer( workerDataExchange , buffer );
		
	}
	
	void WorkerTaskManager::addFile( size_t task_id , network::QueueFile &qf , Buffer *buffer)
	{
		
		// Create the task
		WorkerTask *t = task.findInMap( task_id );

		if( t )
			t->addFile( qf , buffer );
		else
			Engine::shared()->memoryManager.destroyBuffer( buffer );
		
	}
	
	
	void WorkerTaskManager::finishWorker( size_t task_id )
	{
		
		// Create the task
		WorkerTask *t = task.findInMap( task_id );

		if( t )
		{
			t->finishWorker();
			if( t->status == WorkerTask::completed )
			{
				// Remove the tasks from the task manager
				delete task.extractFromMap( task_id );
			}
		}
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
	
	
		
	void WorkerTaskManager::notifyFinishProcess( ProcessItem * item )
	{
		
		size_t task_id = item->tag;
		WorkerTask *t = task.findInMap( task_id );
		if( t )
		{
			t->notifyFinishProcess( item );
			if( t->status == WorkerTask::completed )
			{
				// Remove the tasks from the task manager
				delete task.extractFromMap( task_id );
			}
		}
		delete item;
	}
	
	
	void WorkerTaskManager::diskManagerNotifyFinish(  DiskOperation *operation )
	{
		switch (operation->getType()) {
				
			case DiskOperation::read:
			{
				size_t task_id = operation->tag;
				WorkerTask *t = task.findInMap( task_id );
				if( t )
				{
					if( operation->error.isActivated() )
						t->setError(operation->error.getMessage());
					else
						t->diskManagerNotifyFinish( operation );
					
					if( t->status == WorkerTask::completed )
					{
						// Remove the tasks from the task manager
						delete task.extractFromMap( task_id );
					}
				}
			}
				
				break;
			case DiskOperation::write:
			{
				size_t task_id = operation->tag;
				WorkerTask *t = task.findInMap( task_id );
				if( t )
				{
					if( operation->error.isActivated() )
						t->setError(operation->error.getMessage());
					else
						t->diskManagerNotifyFinish( operation );
					
					if( t->status == WorkerTask::completed )
					{
						// Remove the tasks from the task manager
						delete task.extractFromMap( task_id );
					}
				}
			}
				break;
			case DiskOperation::remove:
				break;
			default:
				break;
		}		
		
	}
	

	
	void WorkerTaskManager::notifyFinishMemoryRequest( MemoryRequest *request )
	{
		size_t task_id = request->tag;
		WorkerTask *t = task.findInMap( task_id );
		if( t )
		{
			t->notifyFinishMemoryRequest( request );
			if( t->status == WorkerTask::completed )
			{
				// Remove the tasks from the task manager
				delete task.extractFromMap( task_id );
			}
		}
	}

	
	
	std::string WorkerTaskManager::getStatus()
	{
		
		std::ostringstream output;
		
		std::map<size_t,WorkerTask*>::iterator iter;
		
		output << "\n";
		for ( iter = task.begin() ; iter != task.end() ; iter++)
			output << "\t\t\t[" << iter->second->getStatus() << "]\n";
		
		return output.str();
	}
	
	
}
