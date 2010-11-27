#ifndef _H_WORKER_TASK_MANAGER
#define _H_WORKER_TASK_MANAGER

#include <list>						// std::list

#include "au_map.h"					// au::map
#include "samson.pb.h"				// WorkerTask
#include "ObjectWithStatus.h"
#include "Lock.h"					// au::Lock
#include "samson.pb.h"				// ss::network::...

namespace ss {

	class SamsonWorker;
	class DataBufferItem;
	class WorkerTask;
	class WorkerTaskItem;
	
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

		// Noitification received from the DataBuffer when everything is saved to disk
		void completeTask( size_t task_id );
		
		std::string getStatus();
		
	private:
		
		// Function used to send the confirmation of a task to the controller
		void sendWorkTaskConfirmation( WorkerTask *t );
		
		// Function used to send the "close" message to other workers
		void sendCloseMessages( WorkerTask *t , int workers );
		
		
	};
}

#endif
