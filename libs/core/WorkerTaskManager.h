#ifndef _H_WORKER_TASK_MANAGER
#define _H_WORKER_TASK_MANAGER

#include <list>						// std::list

#include "au_map.h"					// au::map
#include "samson.pb.h"				// WorkerTask
#include "Status.h"
#include "Lock.h"					// au::Lock
#include "samson.pb.h"				// ss::network::...
#include "DiskManagerDelegate.h"	// ss::FileManagerDelegate
#include "Status.h"				// au::Status


namespace ss {

	class SamsonWorker;
	class DataBufferItem;
	class WorkerTask;
	class WorkerTaskItem;
	class ProcessAssistant;
	
	class WorkerTaskManager :  public au::Status
	{
		
	public:
		
		SamsonWorker *worker;

	private:
		au::map<size_t,WorkerTask> task;	// List of tasks
		
		au::Lock lock;			// General lock to protect multiple access ( network thread & Process threads )
		au::StopLock stopLock;	// Stop lock to block the Process when there is no process to run
		
		
		// Relation between file_ids and Items ( when receiving notifications form File Manager )
		au::map<size_t, WorkerTaskItem > pendingInputFiles;

		int num_processes;
		ProcessAssistant**   processAssistant;  // vector of core worker processes
		
		friend class WorkerTaskItem;
		
	public:
		
		WorkerTaskManager(SamsonWorker *_worker);

		// Add a task to the local task manager ( map / reduce / generator )
		void addTask( const network::WorkerTask &task );

		// Method called from the ProcessAssitant to get the next element to process
		WorkerTaskItem *getNextItemToProcess();
		
		// Method called by ProcessAssitant when a particula process is finished
		void finishItem( WorkerTaskItem *item );

		// Notification received from the DataBuffer when everything is saved to disk
		void completeTask( size_t task_id );
		
		// Get information about internal status
		void getStatus( std::ostream &output , std::string prefix_per_line );
		
		// Fill a confirmation message
		void fill( size_t task_id , network::WorkerTaskConfirmation *confirmation );
		
		// Full with information about status
		void fill(network::WorkerStatus*  ws);
		
		void wakeUp()
		{
			lock.wakeUpStopLock( &stopLock );
		}
		
	private:
		
		// Function used to send the confirmation of a task to the controller
		void sendWorkTaskConfirmation( WorkerTask *t );
		
		// Function used to send the "close" message to other workers
		void sendCloseMessages( size_t task_id , int workers );
		
		// Function calles by items when adding files to be read
		void addInputFile( size_t fm_id , WorkerTaskItem* );

		// Setup all tasks ( when a shared memory area is free )
		void _setupAllTasks();
		
		
	};
}

#endif
