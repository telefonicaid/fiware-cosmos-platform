#include "logMsg.h"               // LM_*
#include "traceLevels.h"          // Trace Levels

#include "iomMsgSend.h"           // iomMsgSend
#include "CommandLine.h"          // au::CommandLine
#include "SamsonWorker.h"         // SamsonWorker
#include "WorkerTaskManager.h"    // Own interface
#include "Packet.h"               // ss::Packet
#include "DataBufferItem.h"       // ss::DataBufferItem

#include "WorkerTask.h"           // ss::WorkerTask
#include "SamsonSetup.h"          // ss::SamsonSetup
#include "FileManagerReadItem.h"  // ss::FileManagerReadItem
#include "FileManagerWriteItem.h" // ss::FileManagerWriteItem



namespace ss {
	
	WorkerTaskManager::WorkerTaskManager(SamsonWorker* _worker)
	{
		worker = _worker;
	}
	
	void WorkerTaskManager::addTask(const network::WorkerTask &worker_task )
	{
		
		token.retain();

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
		
		token.release();

	}
	
	void WorkerTaskManager::killTask( const network::WorkerTaskKill &task_kill )
	{
		token.retain();
		
		// Create the task
		WorkerTask *t = task.extractFromMap( task_kill.task_id() );

		if( t )
		{
			t->kill();
			delete t;
		}
		
		token.release();
	}
	
	
	void WorkerTaskManager::addBuffer( size_t task_id , network::Queue queue , Buffer* buffer , bool txt  )
	{
		token.retain();
		
		// Create the task
		WorkerTask *t = task.findInMap( task_id );
		if( !t )
			t = new WorkerTask( this );

		// Add the buffer to the task item
		t->addBuffer( queue , buffer , txt );
		
		token.release();
		
	}
	
	void WorkerTaskManager::addFile( size_t task_id , network::QueueFile &qf , Buffer *buffer)
	{
		token.retain();
		
		LM_TODO(("Solve this ;)"));
		
		// Create the task
		WorkerTask *t = task.findInMap( task_id );

		if( t )
			t->addFile( qf , buffer );
		else
			MemoryManager::shared()->destroyBuffer( buffer );
		
		token.release();		
	}
	
	
	void WorkerTaskManager::finishWorker( size_t task_id )
	{
		token.retain();
		
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
		
		token.release();
	}

	void WorkerTaskManager::fill(network::WorkerStatus*  ws)
	{
		ws->set_task_manager_status( getStatus() );
	}
	
	// Check if a particular task is still active
	bool WorkerTaskManager::checkTask( size_t task_id )
	{
		token.retain();
		bool tmp = ( task.findInMap( task_id ) != NULL);
		token.release();
		
		return tmp;
	}
	
	
		
	void WorkerTaskManager::notifyFinishProcess( ProcessItem * item )
	{
		
		token.retain();
		
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
		
		token.release();
	}
	
	void WorkerTaskManager::notifyFinishReadItem( FileManagerReadItem *item  )
	{
		token.retain();
		size_t task_id = item->tag;
		WorkerTask *t = task.findInMap( task_id );
		if( t )
		{
			if( item->error.isActivated() )
				t->setError(item->error.getMessage());
			else
				t->notifyFinishReadItem( item );
			
			if( t->status == WorkerTask::completed )
			{
				// Remove the tasks from the task manager
				delete task.extractFromMap( task_id );
			}
		}
		delete item;
		
		token.release();
	}
	
	void WorkerTaskManager::notifyFinishWriteItem( FileManagerWriteItem *item  )
	{
		token.retain();
		
		size_t task_id = item->tag;
		WorkerTask *t = task.findInMap( task_id );
		if( t )
		{
			if( item->error.isActivated() )
				t->setError(item->error.getMessage());
			else
				t->notifyFinishWriteItem( item );
			
			if( t->status == WorkerTask::completed )
			{
				// Remove the tasks from the task manager
				delete task.extractFromMap( task_id );
			}
		}
		delete item;
		
		token.release();
		
	}
	
	void WorkerTaskManager::notifyFinishMemoryRequest( MemoryRequest *request )
	{
		token.retain();
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

		delete request;
				
		token.release();
		
	}

	
	
	std::string WorkerTaskManager::getStatus()
	{
		
		std::ostringstream output;
		
		token.retain();
		
		std::map<size_t,WorkerTask*>::iterator iter;
		
		output << "\n";
		for ( iter = task.begin() ; iter != task.end() ; iter++)
			output << "\t\t\t[" << iter->second->getStatus() << "]\n";
		
		token.release();
		
		return output.str();
	}
	
	
}
