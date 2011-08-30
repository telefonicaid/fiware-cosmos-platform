
#ifndef _H_WORKER_TASK
#define _H_WORKER_TASK

#include <cstring>				// size_t
#include <string>				// std::string
#include <set>					// std::set

#include "au/map.h"				// au::map
#include "au/ErrorManager.h"			// au::ErrorManager
#include "au/Token.h"			// au::token

#include "engine/Object.h" // samson::EngineNotificationListener
#include "engine/Buffer.h"
#include "engine/MemoryRequest.h"  
#include "engine/DiskOperation.h"  
#include "engine/ProcessItem.h"

#include "engine/OperationsContainer.h"       // engine::OperationsContainer


#include "samson/common/samson.pb.h"			// samson::network::...
#include "samson/common/coding.h"             // samson::KVInfo

#include "samson/module/Operation.h"	// samson::Operation
#include "samson/module/Environment.h"	// samson::Environment

#define notification_sub_task_finished      "notification_sub_task_finished"

namespace samson {

	class WorkerTaskManager;
	class ProcessAssistantSharedFileCollection;
	class ProcessItem;
	class WorkerSubTask;
	class QueueuBufferVector;
	class DataBufferProcessItem;
	
    
    /*
     Class to store information about the generated file
     */
    
    class WorkerTaskOutputFile
    {
        
    public:
        
        std::string fileName;       // Name of the file
        std::string queue;          // Name of the queue
        int worker;                 // Worker that generated this file
        KVInfo info;                // Global information for this file
        
        WorkerTaskOutputFile( std::string _fileName , std::string _queue , int _worker )
        {
            fileName = _fileName;
            queue = _queue;
            worker = _worker;
            
            info.clear();
        }
        
        void fill( network::QueueFile *qf )
        {
            // Fill information in this GPB structure
            qf->set_queue( queue );
            network::File *file = qf->mutable_file();
            file->set_name( fileName );
            file->set_worker( worker );
            network::KVInfo * network_info = file->mutable_info();
            network_info->set_size( info.size );
            network_info->set_kvs( info.kvs );
        }
        
    };
    
	class WorkerTask : public engine::Object , public engine::OperationsContainer
	{
		
        
		// List of subtasks
        au::map<size_t,WorkerSubTask> subTasks;
        
		// Error management
		au::ErrorManager error;
        
	public:
		
		typedef enum
		{
			pending_definition,			// Pending to receive message from the controller
			running,					// Running operation
			local_content_finished,		// Output content is completed ( a message is send to the other workers to notify ) 
			all_content_finish,			// The content from all the workers is received ( file are starting to be saved )
			finish,						// All the output files are generated ( not saved ). Controller is notified about this to continue scripts
			completed,					// Output content is saved on disk ( task can be removed from task manager )
		} WorkerTaskStatus;
		
        
		WorkerTaskManager *taskManager;         // Pointer to the task manager
		size_t job_id;                         // identifier of the task
 		size_t task_id;                         // identifier of the task
        size_t subTaskId;                       // Internal counter to give a number to each sub-task
		WorkerTaskStatus status;                // Status of this task
		Operation::Type type;                   // Type of operation
		std::string operation;                  // Name of the operation
        
        
		// Number of workers that confirmed the end of data
		int num_workers;
		int num_finished_workers;

        // Number of buffer to process
        int num_process_items;

        // Number of pending disk operations
        int num_disk_operations;
        
        // Number of pending sub-tasks
        int num_subtasks;
        
		// Message from and to the controller
		network::WorkerTask workerTask;                             // Copy of the message received from the controller
		network::WorkerTaskConfirmation *complete_message;			// Message prepared to be send to the controller ( complete task : all saved )
		
		// Common information for this task
		ProcessAssistantSharedFileCollection *reduceInformation;
		
		// xml information
        void getInfo( std::ostringstream& output);
		
		// Set of vectorBuffers for each output queue
		au::map<std::string , QueueuBufferVector> queueBufferVectors;		// Buffer of vector for key-value outputs ( buffered in memory for performance )	
        au::map<std::string , WorkerTaskOutputFile> outputFiles;            // List of the output files to be used when reporting the finish message
        au::map<std::string , WorkerTaskOutputFile> outputRemoveFiles;      // List of the output files that should be removed ( compact operation )
        
		public:

		// Constructor and destructor
		WorkerTask(WorkerTaskManager *taskManager );
		~WorkerTask();

		// Setup with the information comming from the controller
		void setupAndRun( Operation::Type type , const network::WorkerTask &task );	
		
        // Set error
		void setError(std::string _error_message);
		
		// Kill( from a message from the controller )
		void kill( std::string message );
		
		// Processign income buffers
		void addBuffer( network::WorkerDataExchange& workerDataExchange , engine::Buffer *buffer );

        // Notification that a particular worker has finished generating data
        void finishWorker( int worker_from );
        
        // General notification function
        void notify( engine::Notification* notification );

        
    public:
        
        void addKVFile( std::string fileName , std::string queue , engine::Buffer *buffer );   // Only key-value vectors
        
 		void addFile(  std::string fileName , std::string queue , KVInfo info , engine::Buffer *buffer);    // Add registration of this file and notify the disk operation
        void addFile( std::string fileName , std::string queue , KVInfo info );                     // Basic call to add registration
		void removeFile(  std::string fileName , std::string queue );

		// Add subtasks
		void addSubTask( WorkerSubTask *subTask );
		void flush();

        
        int num_file_output;    // Incremental value to give names to the files
		std::string newFileName( );
		std::string newFileNameForTXTOutput( int hg_set );
        
    private:

        void addDiskOperation( engine::DiskOperation *operation );
        void addProcessItem( engine::ProcessItem *item );
        
        void setNotificationCommonEnvironment( engine::Notification*notification );
        
    public:
        
        bool canBeRemoved();
        
#pragma mark Messages
		
		void sendCloseMessages();
		void sendFinishTaskMessageToController( );		
		void sendCompleteTaskMessageToController( );		
    public:
        void sendUpdateMessageToController( FullKVInfo running_info , FullKVInfo processed_info );

        
        
#pragma mark Check
		
	private:
		
		// Internal function to verytfy the internal status of this task ( and change status if necessary )
		void check();

	};

	
}
#endif
