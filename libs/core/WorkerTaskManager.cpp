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
		num_processes	= SamsonSetup::shared()->num_processes;			// Define the number of process
		
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
		
		bool send_close_messages = false;
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
			send_close_messages = true;
		
		
		lock.unlock();
		
		// Wake up ProcessAssitant to process items ( really not necesssary since there is a 1 second timeout )
		lock.wakeUpAllStopLock( &stopLock );

		if( send_close_messages )
			sendCloseMessages( t->getId() , worker->network->getNumWorkers() );

		
		
	}

	WorkerTaskItem *WorkerTaskManager::getNextItemToProcess()
	{
		while( true )
		{
			lock.lock();
			
			_setupAllTasks();
			
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
			
			lock.unlock_waiting_in_stopLock( &stopLock , 2 );
		}
		
	}	
	
	/*
	 Notify that a particular item has finished producing data
	 */
	
	void WorkerTaskManager::finishItem( WorkerTaskItem *item )
	{
		
		bool send_close_messages = false;
		int num_finished_items = 0;
		int num_items = 0;

		size_t task_id = item->task->task_id ;
		
		lock.lock();
		
		WorkerTask *t = task.findInMap( task_id );
		assert( t );

		// Notify about this finish
		t->finishItem( item->item_id );

		num_items = t->num_items;
		num_finished_items = t->num_finish_items;
		
		// If all have finished, send the close message
		if( t->isFinish() )
			send_close_messages = true;

		// The task is not defined complete until a close is received from all workers.
		// It is responsability of DataBuffer to notify WorkerTaskManager about this to finally remove the task
		
		// setup all tasks to see if resources can be re-used
		_setupAllTasks();
		
		lock.unlock();

		// Send a message to controller to update status of this task
		send_update_message_to_controller(worker->network, task_id, num_finished_items, num_items );
		
		if( send_close_messages )
			sendCloseMessages( t->getId() , worker->network->getNumWorkers() );
		
		
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
		
		WorkerTask *t = task.extractFromMap( task_id );

		if( t )
		{
			assert( t->isFinish() );	// Otherwise it is not possible to be here
			delete t;
		}
		
		lock.unlock();
		
		send_complete_task_message_to_controller(worker->network, task_id);
		
	}
	

	void WorkerTaskManager::fill(network::WorkerStatus*  ws)
	{
		ws->set_task_manager_status( getStatus() );
		
		for (int i = 0 ; i < num_processes ; i++)
		{
			network::WorkerProcess *p = ws->add_process();
			p->set_status( processAssistant[i]->getStatus() );
		}
	}
	
	void WorkerTaskManager::sendCloseMessages( size_t task_id , int workers )
	{
		
		for (int s = 0 ; s < workers ; s++)
		{				
			Packet *p = new Packet();
			network::WorkerDataExchangeClose *dataMessage =  p->message.mutable_data_close();
			dataMessage->set_task_id(task_id);
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
	
	std::string WorkerTaskManager::getStatus()
	{
		std::ostringstream output;
		
		lock.lock();
		
		std::map<size_t,WorkerTask*>::iterator iter;
		
		for ( iter = task.begin() ; iter != task.end() ; iter++)
			output << "[" << iter->second->getStatus() << "]";
		lock.unlock();
		
		return output.str();
	}

	
	
	void WorkerTaskManager::addInputFile( size_t fm_id , WorkerTaskItem* item )
	{
		pendingInputFiles.insertInMap( fm_id , item );
	}

	void WorkerTaskManager::send_finish_task_message_to_controller(NetworkInterface *network , size_t task_id )
	{		
		Packet *p = new Packet();
		network::WorkerTaskConfirmation *confirmation = p->message.mutable_worker_task_confirmation();
		confirmation->set_task_id( task_id );
		confirmation->set_type( network::WorkerTaskConfirmation::finish );
		
		network->send( NULL, network->controllerGetIdentifier(), Message::WorkerTaskConfirmation, p);
	}

	void WorkerTaskManager::send_complete_task_message_to_controller(NetworkInterface *network , size_t task_id )
	{		
		Packet *p = new Packet();
		network::WorkerTaskConfirmation *confirmation = p->message.mutable_worker_task_confirmation();
		confirmation->set_task_id( task_id );
		confirmation->set_type( network::WorkerTaskConfirmation::complete );
		
		network->send( NULL, network->controllerGetIdentifier(), Message::WorkerTaskConfirmation, p);
	}
	
	void WorkerTaskManager::send_add_file_message_to_controller(NetworkInterface *network , size_t task_id , const network::QueueFile &qf )
	{		
		Packet *p = new Packet();
		network::WorkerTaskConfirmation *confirmation = p->message.mutable_worker_task_confirmation();
		confirmation->set_task_id( task_id );
		confirmation->set_type( network::WorkerTaskConfirmation::new_file );
		confirmation->add_file()->CopyFrom( qf );
		network->send( NULL, network->controllerGetIdentifier(), Message::WorkerTaskConfirmation, p);
	}
	
	void WorkerTaskManager::send_update_message_to_controller(NetworkInterface *network , size_t task_id ,int num_finished_items, int num_items )
	{		
		Packet *p = new Packet();
		network::WorkerTaskConfirmation *confirmation = p->message.mutable_worker_task_confirmation();
		confirmation->set_task_id( task_id );
		confirmation->set_type( network::WorkerTaskConfirmation::update );
		confirmation->set_num_items(num_items );
		confirmation->set_num_finished_items( num_finished_items );

		network->send( NULL, network->controllerGetIdentifier(), Message::WorkerTaskConfirmation, p);
	}
	
}
