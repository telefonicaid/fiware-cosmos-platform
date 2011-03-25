#ifndef _H_WORKER_TASK_MANAGER
#define _H_WORKER_TASK_MANAGER

#include <list>							// std::list
#include "au_map.h"						// au::map
#include "samson.pb.h"					// WorkerTask
#include "Token.h"						// au::Token
#include "samson.pb.h"					// ss::network::...
#include "ProcessItem.h"				// ss::ProcessManagerDelegate
#include "Buffer.h"						// ss::Buffer
#include "MemoryRequest.h"				// ss::MemoryRequest & MemoryRequestDelegate
#include "EngineDelegates.h"

namespace ss {


	class SamsonWorker;
	class DataBufferItem;
	class WorkerTask;
	class NetworkInterface;
	
	class WorkerTaskManager : public ProcessManagerDelegate, public DiskManagerDelegate , public MemoryRequestDelegate		// Receive notifications from the process manager
	{
		
	public:
		
		SamsonWorker *worker;

	private:

		au::map<size_t,WorkerTask> task;					// List of tasks
				
	public:
		
		WorkerTaskManager(SamsonWorker *_worker);

		// Add a task from a message comming from the controller
		void addTask( const network::WorkerTask &task );

		// Kill a task
		void killTask( const network::WorkerTaskKill &task_kill );
		
		// Add a buffer from other workers ( associated to a particular task and output queue)
		void addBuffer( size_t task_id , network::WorkerDataExchange& workerDataExchange , Buffer* buffer );
		
		// Add a file directly to a task
		void addFile( size_t task_id , network::QueueFile &qf , Buffer *buffer);
		
		// Notification that a worker has finish produccing data for a task
		void finishWorker( size_t task_id );

		// Full with information about status
		void fill(network::WorkerStatus*  ws);
		
		// Check if a particular task is still active
		bool checkTask( size_t task_id );
		
		// Get a debugging string
		std::string getStatus();

#pragma mark Notifications from Process and FileManager and MemoryManager
		
	public:
		
		void notifyFinishProcess( ProcessItem * item );
		
		void notifyFinishMemoryRequest( MemoryRequest *request );		
		
		void diskManagerNotifyFinish(  DiskOperation *operation );	
		
	};
	 
}

#endif
