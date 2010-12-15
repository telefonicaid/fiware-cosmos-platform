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
#include "ProcessAssistant.h"	// ss::ProcessAssistant

#include "SamsonSetup.h"		// ss::SamsonSetup

namespace ss {
	
	WorkerTaskManager::WorkerTaskManager(SamsonWorker *_worker) : stopLock( &lock )
	{
		worker = _worker;
		num_processes = SamsonSetup::shared()->num_processes;			// Define the number of process

		
		// //////////////////////////////////////////////////////////////////////
		//
		// Create one ProcessAssistant per core
		//
		if (num_processes == -1)
			LM_X(1, ("Invalid number of cores. Please edit /opt/samson/setup.txt"));
		
		LM_T(LMT_WINIT, ("initializing %d process assistants", num_processes));
		processAssistant = (ProcessAssistant**) calloc(num_processes, sizeof(ProcessAssistant*));
		if (processAssistant == NULL)
			LM_XP(1, ("calloc(%d, %d)", num_processes, sizeof(ProcessAssistant*)));
		
		int coreId;
		for (coreId = 0; coreId < num_processes ; coreId++)
		{
			processAssistant[coreId] = new ProcessAssistant(coreId, this);
		}
		
		LM_T(LMT_WINIT, ("Got %d process assistants", coreId));
		
		// Setup run-time status 
		setStatusTile( "Task Manager" , "tm" );

		
	}
	
	void WorkerTaskManager::addTask(const network::WorkerTask &worker_task )
	{
		lock.lock();

		// Look at the operation to 
		Operation *op = worker->modulesManager.getOperation( worker_task.operation() );
		assert( op );		// TODO: Better handling of no operation error

		// Create the task
		WorkerTask *t = new WorkerTask( this, op->getType() , worker_task );

		// Insert into internal map
		task.insertInMap( t->task_id , t );
		
		// If it is already finished (this happens where there are no input files )
		if( t->isFinish() )
			sendCloseMessages( t , worker->network->getNumWorkers() );
		
		
		lock.unlock();
		
		// Wake up ProcessAssitant to process items ( really not necesssary since there is a 1 second timeout )
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
			
			lock.unlock_waiting_in_stopLock( &stopLock );
		}
		
	}	
	
	/*
	 Notify that a particular item has finished producing data
	 */
	
	void WorkerTaskManager::finishItem( WorkerTaskItem *item )
	{
		lock.lock();
		
		WorkerTask *t = task.findInMap( item->task->task_id );
		assert( t );

		// Notify about this finish
		t->finishItem( item->item_id );
		
		// If all have finished, send the close message
		if( t->isFinish() )
			sendCloseMessages( t , worker->network->getNumWorkers() );

		// The task is not defined complete until a close is received from all workers.
		// It is responsability of DataBuffer to notify WorkerTaskManager about this to finally remove the task
		
		// setup all tasks to see if resources can be re-used
		_setupAllTasks();

		
		// Send a confirmation message to the controller to keep the track of items
		Packet *p = new Packet();
		network::WorkerTaskConfirmation *confirmation = p->message.mutable_worker_task_confirmation();
		confirmation->set_task_id( t->task_id );			
		confirmation->set_finish( false );
		confirmation->set_completed( false );			
		confirmation->set_num_items( t->num_items );
		confirmation->set_num_finish_items( t->num_finish_items );
		confirmation->set_error( t->error );
		confirmation->set_error_message( t->error_message );			
		worker->network->send(worker, worker->network->controllerGetIdentifier(), Message::WorkerTaskConfirmation, p);
		
		lock.unlock();
		
		lock.wakeUpStopLock( &stopLock );
		
	}	
	
	/**
	 Setup all tasks to see if any of them needs a shared memory area used by the item that has just finished
	 */
	
	void WorkerTaskManager::_setupAllTasks()
	{
		std::map<size_t,WorkerTask*>::iterator t;
		for (t = task.begin() ; t!= task.end() ; t++)
			t->second->setup();
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
			confirmation->set_num_items( t->num_items );
			confirmation->set_num_finish_items( t->num_finish_items );
			confirmation->set_error_message( t->error_message );			
			worker->network->send(worker, worker->network->controllerGetIdentifier(), Message::WorkerTaskConfirmation, p);
			
			assert( t->isFinish() );	// Otherwise it is not possible to be here
			delete t;
		}
		
		lock.unlock();
		
	}
	

	void WorkerTaskManager::fill(network::WorkerStatus*  ws)
	{
		for (int i = 0 ; i < num_processes ; i++)
		{
			network::WorkerProcess *p = ws->add_process();
			p->set_status( processAssistant[i]->getStatus() );
		}
	}
	
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
	
	// Funciton to get the run-time status of this object
	void WorkerTaskManager::getStatus( std::ostream &output , std::string prefix_per_line )
	{
		output << "\n";

		getStatusFromMap( output, task , prefix_per_line );

		output << prefix_per_line << "ProcessAssistants: (" << num_processes << " process):\n";
		for (int i = 0 ; i < num_processes ; i++)
			output << prefix_per_line << "\t" << processAssistant[i]->getStatus() << std::endl; 
		
	}
	
	
	void WorkerTaskManager::addInputFile( size_t fm_id , WorkerTaskItem* item )
	{
		pendingInputFiles.insertInMap( fm_id , item );
	}

	void WorkerTaskManager::fill( size_t task_id , network::WorkerTaskConfirmation *confirmation )
	{
		lock.lock();
		
		WorkerTask *t = task.findInMap( task_id );
		
		if( t )
		{
			confirmation->set_num_items( t->num_items );
			confirmation->set_num_finish_items( t->num_finish_items );
		}
		
		lock.unlock();
	}
	
	
	
}
