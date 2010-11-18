#include "logMsg.h"              // LM_*
#include "coreTracelevels.h"     // LMT_*

#include "iomMsgSend.h"          // iomMsgSend
#include "CommandLine.h"         // au::CommandLine
#include "SamsonWorker.h"        // SamsonWorker
#include "WorkerTaskManager.h"   // Own interface
#include "Packet.h"				 // ss::Packet


namespace ss {

	
	
#pragma mark WORKER TASK
	
	void WorkerTask::finishItem( size_t id , bool _error , std::string _error_message )
	{
		WorkerTaskItem *i = item.extractFromMap( id );
		delete i;
		
		if( _error )
		{
			error = true;
			error_message = _error_message;
		}
	}	
	
#pragma mark WORKER TASK MANAGER
	
	void WorkerTaskManager::addTask(const network::WorkerTask &worker_task )
	{
		lock.lock();

		WorkerTask *t = new WorkerTask( worker_task );
		
		// Debuggin: Instead of adding the task, just answer back to the controller
		//sendWorkTaskConfirmation(t);
		//delete t;
		
		task.insertInMap( t->task_id , t );
		
		lock.unlock();
		
		// Wake up ProcessAssitant to process items
		lock.wakeUpAllStopLock( &stopLock );	
		
	}

	WorkerTaskItem *WorkerTaskManager::getNextItemToProcess()
	{
		while( true )
		{
			lock.lock();
			
			// Iterate the task list
			std::map<size_t,WorkerTask*>::iterator t;
			for (t = task.begin() ; t!= task.end() ; t++)
			{
				WorkerTaskItem *item = t->second->getNextItemToProcess();
				
				if( item )
				{
					item->startProcess();
					lock.unlock();
					return item;
				}
			}
			
			lock.unlock_waiting_in_stopLock( &stopLock );
		}
		
	}	
	
	void WorkerTaskManager::finishItem( WorkerTaskItem *item , bool error, std::string error_message)
	{
		lock.lock();
		
		WorkerTask *t = task.findInMap( item->task_id );
		assert( t );
		t->finishItem( item->item_id , error, error_message );
		
		if( t->isFinish() )
		{
			
			// Notify controller that this task is finish ( with or without error ) 
			// This confirmation has to be send when all data is received from all workers
			sendWorkTaskConfirmation( t );
			
			// Remove the task from the task manager
			WorkerTask *t = task.extractFromMap( item->task_id  );
			delete t;
		}
		
		lock.unlock();
	}	
	
	
	void WorkerTaskManager::sendWorkTaskConfirmation( WorkerTask *t )
	{
		Packet p;
		network::WorkerTaskConfirmation *confirmation = p.message.mutable_worker_task_confirmation();
		confirmation->set_task_id(  t->task_id );
		confirmation->set_error( t->error );
		confirmation->set_error_message( t->error_message );
		worker->network->send(worker, worker->network->controllerGetIdentifier(), Message::WorkerTaskConfirmation, &p);
		
	}
	
	
	
#if 0
	// Fill information about status of this worker
	void WorkerTaskManager::fillWorkerStatus( network::WorkerStatus* status )
	{
		network::WorkerSystem *system =  status->mutable_system();
		system->set_cores_total(10);
		system->set_cores_running(1);
	}
#endif
	
	
	
	
}
