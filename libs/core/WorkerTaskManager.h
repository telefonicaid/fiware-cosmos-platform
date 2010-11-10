#ifndef _H_WORKER_TASK_MANAGER
#define _H_WORKER_TASK_MANAGER

#include "SamsonWorker.h"			// SamsonWorker
#include "samson.pb.h"				// WorkerTask
#include <list>						// std::list
#include "au_map.h"					// au::map

namespace ss {

	class SamsonWorker;

	/**
	 Class describing a part of a task.
	 This item can be executed by a particular Process
	 */
	
	class WorkerTaskItem
	{
	public:
		
		size_t task_id;	// Global task id
		int item_id;	// Item id in this worker
			
		std::string operation;
		
		enum Status
		{
			definition,
			ready,
			running
		};
		
		Status status;
		
		
		WorkerTaskItem( size_t _task_id , size_t _item_id ,  std::string _operation  )
		{
			operation = _operation;
			
			task_id = _task_id;
			item_id = _item_id;
			
			status = definition;
			status = ready;	// To be automatically accepted now
			
		}
		
		Status getStatus()
		{
			return status;
		}
		
		void startProcess()
		{
			assert( status == ready );
			status = running;
		}
		
		bool isReady()
		{
			return (status == ready);
		}
		
	};
	
	class WorkerTask
	{
		
	public:
		
		size_t task_id;
		
		bool error;
		std::string error_message;
		
		std::string operation;					// Operation to be executed
		au::map<size_t,WorkerTaskItem> item;	// List of items to be executed by processes

		int num_finish_items;
		
		WorkerTask( const network::WorkerTask &task )
		{
			operation = task.operation();	// Save the operation to perform		
			task_id = task.task_id();		// Save the task id
			
			// By default no error
			error = false;
			
			// rigth not for demo, we create 10 items to be executed
			for (size_t i = 0 ; i < 10 ; i++)
				item.insertInMap( i , new WorkerTaskItem( task_id , i ,  operation ) );
		}

		// Get the next item ( if any )
		
		WorkerTaskItem *getNextItemToProcess()
		{
			std::map<size_t,WorkerTaskItem*>::iterator iterator;
			for (iterator = item.begin() ; iterator != item.end() ; iterator++)
			{
				WorkerTaskItem *item = iterator->second;
				if( item->isReady() )
					return item;
			}
				
			return NULL;
		}
		
		void finishItem( size_t id , bool _error , std::string _error_message );
		
		bool isFinish()
		{
			return ( item.size() == 0);	// No more items to process
		}
		
		
	};
	
	class WorkerTaskManager
	{
		SamsonWorker *worker;

		au::map<size_t,WorkerTask> task;	// List of tasks
		
		au::Lock lock;			// General lock to protect multiple access ( network thread & Process threads )
		au::StopLock stopLock;	// Stop lock to block the Process when there is no process to run
		
	public:
		
		WorkerTaskManager(SamsonWorker *_worker) : stopLock( &lock )
		{
			worker = _worker;
		}

		// Add a task to the local task manager ( map / reduce / generator )
		void addTask( const network::WorkerTask &task );

		// Method called from the ProcessAssitant to get the next element to process
		WorkerTaskItem *getNextItemToProcess();
		
		// Method called by ProcessAssitant when a particula process is finished
		
		void finishItem( WorkerTaskItem *item , bool error, std::string error_message );

	private:
		
		// Function used to send the confirmation of a task to the controller
		void sendWorkTaskConfirmation( WorkerTask *t );
		
	};
}

#endif
