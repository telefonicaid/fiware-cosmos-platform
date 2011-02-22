#ifndef _H_WORKER_TASK_MANAGER
#define _H_WORKER_TASK_MANAGER

#include <list>						// std::list

#include "au_map.h"					// au::map
#include "samson.pb.h"				// WorkerTask
#include "Status.h"
#include "Token.h"					// au::Token
#include "samson.pb.h"				// ss::network::...
#include "DiskManagerDelegate.h"	// ss::FileManagerDelegate
#include "Status.h"				// au::Status
#include "ProcessItem.h"		// ss::ProcessManagerDelegate
#include "Buffer.h"				// ss::Buffer
#include "MemoryManager.h"		// ss::MemoryRequestDelegate

namespace ss {


	class SamsonWorker;
	class DataBufferItem;
	class WorkerTask;
	class NetworkInterface;
	
	class WorkerTaskManager : public ProcessManagerDelegate, public FileManagerDelegate , public MemoryRequestDelegate		// Receive notifications from the process manager
	{
		
	public:
		
		SamsonWorker *worker;

	private:

		au::Token token;					// Token to protect task
		au::map<size_t,WorkerTask> task;	// List of tasks
		
	public:
		
		WorkerTaskManager(SamsonWorker *_worker);

		// Add a task from a message comming from the controller
		void addTask( const network::WorkerTask &task );

		// Kill a task
		void killTask( const network::WorkerTaskKill &task_kill );
		
		// Add a buffer from other workers ( associated to a particular task and output queue)
		void addBuffer( size_t task_id , network::Queue , Buffer* buffer , bool txt  );
		
		// Notification that a worker has finish produccing data for a task
		void finishWorker( size_t task_id );

		// Full with information about status
		void fill(network::WorkerStatus*  ws);
		
		// Get a debugging string
		std::string getStatus();

#pragma mark Notifications from Process and FileManager and MemoryManager
		
	public:
		void notifyFinishProcess( ProcessItem * item );
		void notifyFinishReadItem( FileManagerReadItem *item  );
		void notifyFinishWriteItem( FileManagerWriteItem *item  );
		void notifyFinishMemoryRequest( MemoryRequest *request );
		
		
	public:		
		// Send messages functions 
		static void send_update_message_to_controller(NetworkInterface *network , size_t task_id ,int num_finished_items, int num_items );

		void send_finish_task_message_to_controller(NetworkInterface *network , size_t task_id );
		
	private:
		
		// Function used to send the confirmation of this task to the controller
		void sendWorkTaskConfirmation( WorkerTask *t );
		
	};
	 
}

#endif
