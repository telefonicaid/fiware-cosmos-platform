#ifndef _H_WORKER_TASK_MANAGER
#define _H_WORKER_TASK_MANAGER

#include <list>							// std::list
#include "au/map.h"						// au::map
#include "samson.pb.h"					// WorkerTask
#include "au/Token.h"						// au::Token
#include "samson.pb.h"					// ss::network::...
#include "engine/ProcessItem.h"				// ss::ProcessManagerDelegate
#include "engine/Buffer.h"						// ss::Buffer
#include "engine/MemoryRequest.h"				// ss::MemoryRequest & MemoryRequestDelegate
#include "engine/EngineNotification.h"         // ss::EngineNotification

namespace ss {


	class SamsonWorker;
	class DataBufferItem;
	class WorkerTask;
	class NetworkInterface;
	
    #define notification_task_finished  "notification_task_finished"
    
	class WorkerTaskManager : public engine::NotificationListener 		// Receive notifications from the process manager
	{
		
	public:
		
		SamsonWorker *worker;

	private:

		au::map<size_t,WorkerTask> task;					// List of tasks
				
	public:
		
		WorkerTaskManager(SamsonWorker *_worker);
        ~WorkerTaskManager();
        
		// Add a task from a message comming from the controller
		void addTask( const network::WorkerTask &task );

		// Kill a task
		void killTask( const network::WorkerTaskKill &task_kill );
		
		// Add a buffer from other workers ( associated to a particular task and output queue)
		void addBuffer( size_t task_id , network::WorkerDataExchange& workerDataExchange , engine::Buffer* buffer );
				
		// Notification that a worker has finish produccing data for a task
		void finishWorker( int worker_from , size_t task_id );

		// Full with information about status
		void fill(network::WorkerStatus*  ws);
		
		// Check if a particular task is still active
		bool checkTask( size_t task_id );
		
		// Get a debugging string
		std::string getStatus();
	
        // Notification from the engine about finished tasks
        void notify( engine::Notification* notification );
        
        virtual bool acceptNotification( engine::Notification* notification );
        
	};
	 
}

#endif
