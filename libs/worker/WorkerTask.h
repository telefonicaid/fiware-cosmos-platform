
#ifndef _H_WORKER_TASK
#define _H_WORKER_TASK

#include "au_map.h"				// au::map
#include <cstring>				// size_t
#include <string>				// std::string
#include "samson/Operation.h"	// ss::Operation
#include "samson.pb.h"			// ss::network::...
#include "samson/Environment.h"	// ss::Environment
#include "Status.h"				// au::Status

#define WORKER_TASK_COMPONENT_PROCESS				1
#define WORKER_TASK_COMPONENT_DATA_BUFFER_PROCESS	2

namespace ss {

	class WorkerTaskManager;
	class ProcessAssistantSharedFileCollection;
	class ProcessItem;
	class WorkerSubTask;
	class QueueuBufferVector;
	class DataBufferProcessItem;
	class FileManagerWriteItem;
	class FileManagerReadItem;
	class MemoryRequest;
	class Buffer;
	
	class WorkerTask 
	{
		
	public:
		
		typedef enum
		{
			pending_definition,			// Pending to receive message from the controller
			running,					// Running operation
			local_content_finished,		// Output content is completed ( a message is send to the other workers to notify ) 
			all_content_finish,			// The content from all the workers is received ( file are starting to be saved )
			finish,						// All the output files are generated ( not saved ). Controller is notified about this to continue scripts
			completed					// Output content is saved on disk ( task can be removed from task manager )
		} WorkerTaskStatus;
		
		// Number of workers that confirmed the end of data
		int num_workers;
		int num_finished_workers;

		size_t subTaskId;								// Internal counter to give a number to each sub-task
		
		WorkerTaskManager *taskManager;					// Pointer to the task manager

		network::WorkerTask workerTask;					// Copy of the message received from the controller
		
		size_t task_id;									// identifier of the task
		WorkerTaskStatus status;						// Status of this task

		// Common information for this task
		ProcessAssistantSharedFileCollection *reduceInformation;
		
		// Error management
		bool error;
		std::string error_message;
		
		// Operation to be executed
		std::string operation;					
				
		// Debuggin string
		std::string getStatus();

		// Items management
		au::map<size_t , WorkerSubTask> subTasksWaitingForMemory;			
		au::map<size_t , WorkerSubTask> subTasksWaitingForReadItems;			
		au::map<size_t , WorkerSubTask> subTasksWaitingForProcess;			
		
		// Set of vectorBuffers for each output queue
		au::map<std::string , QueueuBufferVector> queueBufferVectors;			

		// Set of items pendign to be process to joint buffers
		std::set<DataBufferProcessItem*> processWriteItems;	
		
		// Set of items pendign to be written
		std::set<FileManagerWriteItem*> writeItems;			
		
		public:

		// Constructor and destructor
		WorkerTask(WorkerTaskManager *taskManager );
		~WorkerTask();

		// Setup with the information comming from the controller
		void setup(Operation::Type type , const network::WorkerTask &task);	
		
#pragma mark Notifications about finish process and IO operations and memory requests
		
		// Notification that a process has finish ( from ProcessManager )
		void notifyFinishProcess( ProcessItem * i );

		void notifyFinishReadItem( FileManagerReadItem *item  );
		void notifyFinishWriteItem( FileManagerWriteItem *item  );

		void notifyFinishMemoryRequest( MemoryRequest *request );
		
		
		
		// Notify that a worker has finished producing data for this task
		void finishWorker( );

		void setError(std::string _error_message)
		{
			error = true;
			error_message = _error_message;

			// Send the confirmation to the controller
			sendCompleteTaskMessageToController();
			
			// Set the flag of completed to cancel this task automatically
			status = completed;
		}
		
		// Processign income buffers
		
		void addBuffer( network::Queue queue , Buffer *buffer ,bool txt );
		void flush( QueueuBufferVector *bv );
		void flush();
		
#pragma mark Messages
		
		void sendCloseMessages();
		void sendFinishTaskMessageToController( );		
		void sendCompleteTaskMessageToController( );
		void sendAddFileMessageToController( QueueuBufferVector *bv ,  std::string fileName , Buffer *b );
		
#pragma mark FileNames
		
		std::string newFileName( std::string queue );

#pragma mark Manager SubTasks
		
		void addSubTask( WorkerSubTask *subTask );

#pragma mark Check
		
	private:
		// Internal function to verytfy the internal status of this task
		void check();

		
		
	};

	
}
#endif
