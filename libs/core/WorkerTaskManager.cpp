#include "logMsg.h"              // LM_*
#include "coreTracelevels.h"     // LMT_*

#include "iomMsgSend.h"          // iomMsgSend
#include "CommandLine.h"         // au::CommandLine
#include "SamsonWorker.h"        // SamsonWorker
#include "WorkerTaskManager.h"   // Own interface
#include "Packet.h"				 // ss::Packet
#include "DataBufferItem.h"		 // ss::DataBufferItem

#include "WorkerTask.h"			// ss::WorkerTask
#include "WorkerTaskItem.h"		// ss::WorkerTaskItem

namespace ss {
	
	
	void WorkerTaskManager::addTask(const network::WorkerTask &worker_task )
	{
		lock.lock();

		// Look at the operation to 
		Operation *op = worker->modulesManager.getOperation( worker_task.operation() );
		assert( op );		// TODO: Better handling of no operation error
		
		WorkerTask *t = new WorkerTask( op->getType() , worker_task );

		task.insertInMap( t->task_id , t );
		
		// If it is already finished (this happens where there are no input files )
		if( t->isFinish() )
			sendCloseMessages( t , worker->network->getNumWorkers() );
		
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
					lock.unlock();
					return item;
				}
			}
			
			lock.unlock_waiting_in_stopLock( &stopLock ,1 );
		}
		
	}	
	
	/*
	 Notify that a particular item has finished producing data
	 */
	
	void WorkerTaskManager::finishItem( WorkerTaskItem *item , bool error, std::string error_message)
	{
		lock.lock();
		
		WorkerTask *t = task.findInMap( item->task_id );
		assert( t );

		// Notify about this finish
		t->finishItem( item->item_id , error, error_message );
		
		// If all have finished, send the close message
		if( t->isFinish() )
			sendCloseMessages( t , worker->network->getNumWorkers() );

		// The task is not defined complete until a close is received from all workers.
		// It is responsability of DataBuffer to notify WorkerTaskManager about this to finally remove the task
		
		lock.unlock();
	}	

	/**
	 Nofity that a particular task has finished ( everything has been saved to disk )
	 */
	
	void WorkerTaskManager::completeTask( size_t task_id )
	{
		lock.lock();

		WorkerTask *t = task.extractFromMap( task_id  );

		if( t )
		{
			Packet *p = new Packet();
			network::WorkerTaskConfirmation *confirmation = p->message.mutable_worker_task_confirmation();
			confirmation->set_task_id( task_id );
			
			confirmation->set_finish( true );
			confirmation->set_completed( true );
			
			confirmation->set_error( t->error );
			confirmation->set_error_message( t->error_message );
			
			worker->network->send(worker, worker->network->controllerGetIdentifier(), Message::WorkerTaskConfirmation, p);
			
			assert( t->isFinish() );	// Otherwise it is not possible to be here
			delete t;
		}
		
		lock.unlock();
		
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
	
	
	void WorkerTaskManager::sendCloseMessages( WorkerTask *t , int workers )
	{
		
		for (int s = 0 ; s < workers ; s++)
		{				
			Packet *p = new Packet();
			network::WorkerDataExchangeClose *dataMessage =  p->message.mutable_data_close();
			dataMessage->set_task_id(t->getId());
			NetworkInterface *network = worker->network;
			network->send(worker, network->workerGetIdentifier(s) , Message::WorkerDataExchangeClose, p);
		}
	}	
	
	std::string WorkerTaskManager::getStatus()
	{
		std::ostringstream output;
		output << getStatusFromArray( task );
		return output.str();
	}
	
	
	
}
