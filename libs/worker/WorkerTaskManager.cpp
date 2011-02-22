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
		Operation *op = worker->modulesManager.getOperation( worker_task.operation() );
		assert( op );		// TODO: Better handling of no operation error

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

	

#if 0	
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
	
	
	void WorkerTaskManager::finishTask( size_t task_id )
	{
		// We only notify the controller about this to continue in the script ( if we are inside a script )
		send_finish_task_message_to_controller( worker->network , task_id );
	}
#endif
	

	void WorkerTaskManager::fill(network::WorkerStatus*  ws)
	{
		ws->set_task_manager_status( getStatus() );
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
			if( item->error )
				t->setError(item->error_message);
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
			if( item->error )
				t->setError( item->error_message );
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
		
		for ( iter = task.begin() ; iter != task.end() ; iter++)
			output << "[" << iter->second->getStatus() << "]";
		
		token.release();
		
		return output.str();
	}

	
#if 0
	void WorkerTaskManager::addInputFile( size_t fm_id , WorkerTaskItem* item )
	{
		pendingInputFiles.insertInMap( fm_id , item );
	}
#endif
	
		
	
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
