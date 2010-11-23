#include "logMsg.h"              // LM_*
#include "coreTracelevels.h"     // LMT_*

#include "iomMsgSend.h"          // iomMsgSend
#include "CommandLine.h"         // au::CommandLine
#include "SamsonWorker.h"        // SamsonWorker
#include "WorkerTaskManager.h"   // Own interface
#include "Packet.h"				 // ss::Packet
#include "DataBufferItem.h"		 // ss::DataBufferItem

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
	
	/*
	 Notify that a particular item has finished producing data
	 */
	
	void WorkerTaskManager::finishItem( WorkerTaskItem *item , bool error, std::string error_message)
	{
		lock.lock();
		
		WorkerTask *t = task.findInMap( item->task_id );
		assert( t );
		t->finishItem( item->item_id , error, error_message );
		
		
		// If all have finished, send the close message
		if( t->isFinish() )
		{
			sendCloseMessages( t , worker->network->getNumWorkers() );
		}
		
		
		
		lock.unlock();
	}	

	/**
	 Nofity that a particular task has finished ( everything has been saved to disk )
	 */
	
	void WorkerTaskManager::completeItem( size_t task_id , DataBufferItem * item )
	{
		lock.lock();

		WorkerTask *t = task.extractFromMap( task_id  );

		if( t )
		{
			Packet *p = new Packet();
			network::WorkerTaskConfirmation *confirmation = p->message.mutable_worker_task_confirmation();
			confirmation->set_task_id(  t->task_id );
			confirmation->set_error( t->error );
			confirmation->set_error_message( t->error_message );
			
			// add files added with the Data Buffer
			for (int i = 0 ; i < (int)item->qfiles.size() ; i++)
			{
				network::QueueFile *qfile = confirmation->add_file( );
				qfile->CopyFrom( item->qfiles[i] );
			}
				
			worker->network->send(worker, worker->network->controllerGetIdentifier(), Message::WorkerTaskConfirmation, p);
			
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
	
	
	
	
}
