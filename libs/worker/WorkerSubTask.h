#ifndef _H_WORKER_SUB_TASK
#define _H_WORKER_SUB_TASK

#include <cstring>                  // size_t
#include <vector>                   // std::vector
#include <string>                   // std::string
#include "engine/EngineNotification.h"     // ss::EngineNotificationListener
#include "au/Error.h"                  // au::Error
#include "engine/Buffer.h"                 // engine::Buffer
#include "engine/MemoryRequest.h"
#include "engine/DiskOperation.h"
#include "engine/ProcessItem.h"
#include "coding.h"                         // KVInfo


namespace ss
{
	
	class WorkerTask;
	class ProcessAssistantSharedFile;
	
	/**
	 ------------------------------------------------------------------------------------------------
	 WorkerSubTask is a subclass describing a particular operation executed inside a WorkerTask
		It could be a generator , map , reduce , organizer , parser , etc...
		All the sub-tasks in SAMSON ecosystem are divided in three steps:	
     
			- Memory request
			- Read operations
			- Process item
	 
		Rigth now, these three steps are executed sequentially and cannot be reverted: 
        i.e. if memory is reserved, it cannot be discarted and start again...
	 ------------------------------------------------------------------------------------------------
	 */
	
	class WorkerSubTask : public engine::NotificationListener
	{

		int num_read_operations;            // Read operations
		int num_read_operations_confirmed;  // Read operations confirmed by DiskManager

        
        int num_processes;                  // Number of Engine ProcessItems to run
        int num_processes_confirmed;        // Number of Engine ProcessItem executed
        
	public:

        engine::Buffer *buffer;                     // Buffer memory obtained from Memory Manager
        
        // Error management
        au::Error error;
        
        KVInfo info;                    // Input data used in this task ( used to report progress to controller )
        
        typedef enum 
        {
            init,                       // In definition
            waiting_memory,             // Waiting for memory
            waiting_reads,              // Waiting for the read operations
            waiting_process,            // Processing ( running itself )
            finished                    // Operation is finished
        } WorkerSubTaskStatus;
        
        WorkerSubTaskStatus status;         // Status of the subtask
		std::string description;            // Short description for debuggin
				
		WorkerTask *task;                   // Pointer to the parent task
		size_t task_id;                     // ID of the parent task
		size_t sub_task_id;                 // ID of the parent task
		
		// Constructor with the parent task
		WorkerSubTask( WorkerTask *_task );
		
		// Destructor has to be virtual ( calling delete from this parent class )
		virtual ~WorkerSubTask();
		
		// Function to get the memory request ( if any )
		virtual size_t getRequiredMemory()
		{
			return 0;
		}

        // Add the read operations ( calls to addReadOperation are expected )
        virtual void run_read_operations(){};

        // Add process ( calls to addProcess are extected here )
        virtual void run_process(){}
        
        // Get a debugging string
        std::string getStatus();
	
        // General notification function ( memory requests / disk operations / process )
        void notify( engine::Notification* notification );

        // Init function to start asking thinks to the engine
        void run();

    protected:
        
        void addMemoryRequest( size_t size );
        void addReadOperation( engine::DiskOperation *operation );
        void addProcess( engine::ProcessItem* processItem );
        
    private:
        
        // Function to check if a particular notification is for me
        bool acceptNotification( engine::Notification *notification );
        void setNotificationCommandEnvironment( engine::Notification *notification);

	};
	
	/*
	------------------------------------------------------------------------------------------------
	 GeneratorSubTask
	 
	 Specific sub-tasks for generator tasks

	------------------------------------------------------------------------------------------------
	*/
	
	class GeneratorSubTask : public WorkerSubTask
	{
		
	public:
		
		GeneratorSubTask( WorkerTask * task  );
		
		void run_process();
	};
	
	/*
	 ------------------------------------------------------------------------------------------------
	 OrganizerSubTask
	 
		Tasks that organize the operationsSubTasks necessary to process a particular data set
		It reads the "info" vectors for ech involved file and decides how to organize
		groups of hash-groups fo efficiency
	 ------------------------------------------------------------------------------------------------
	 */
		
	class OrganizerSubTask : public WorkerSubTask
	{		
	public:
		
		OrganizerSubTask( WorkerTask *task);		
		
		void run_read_operations();
        void run_process();
		
	private:
		
		engine::DiskOperation * getFileMangerReadItem( ProcessAssistantSharedFile* file );
		
	};	

	
	/*
	 ------------------------------------------------------------------------------------------------
	 OperationSubTask
	 
		Basic task for a map, reduce, parseOut
	 
	 ------------------------------------------------------------------------------------------------
	 */
		 
	 class OperationSubTask : public WorkerSubTask
	 {
	 
	 public:

		 int hg_begin;
		 int hg_end;
		 int num_hash_groups;
		 
         int hg_set;    // Identifier of the hash-group
         
		 size_t memory_requested;
         
		 OperationSubTask( WorkerTask * task , int _hg_begin , int _hg_end  );
		 
		 ~OperationSubTask();

		 // Get the nececessary size for the buffer
		 size_t getRequiredMemory();
         void run_read_operations();
         void run_process();
	 
	 };	

	
	
	/*
	 ------------------------------------------------------------------------------------------------
	 ParserSubTask
	 
	 Specific sub-tasks for parser tasks
	 
	 ------------------------------------------------------------------------------------------------
	 */
		
	
	// Subtask that runs a particular sub-tasks like a map,reduce or parserOut
	
	class ParserSubTask : public WorkerSubTask
	{
		
	public:
		
		std::string fileName;		// Input file name
		size_t fileSize;			// Memory requested for the input file
		
		
		ParserSubTask( WorkerTask * task , std::string fileName  );
		
		~ParserSubTask();
		
		size_t getRequiredMemory();
		
		// Function to get all the read operations necessary for this task
		void run_read_operations();
        void run_process();
		
	};	
		
	
	/*
	 ------------------------------------------------------------------------------------------------
	 ParserSubTask
	 
	 Specific sub-tasks system operations ( like compact )
	 
	 ------------------------------------------------------------------------------------------------
	 */
		
	class SystemSubTask : public WorkerSubTask
	{		
	public:
		
		SystemSubTask( WorkerTask *task );		
		
		// Function to get all the read operations necessary for this task
		void run_read_operations();
		
		// Function to get the ProcessManagerItem to run
        void run_process();
		
	private:
		
		engine::DiskOperation * getFileMangerReadItem( ProcessAssistantSharedFile* file );
	};	

	/**
	 Spetial operation to compact files into new set of files
	 */
	
	class CompactSubTask : public WorkerSubTask
	{
		
	public:
		
		int hg_begin;
		int hg_end;
		int num_hash_groups;
		
		size_t memory_requested;
		
		
		CompactSubTask( WorkerTask * task , int _hg_begin , int _hg_end  );
		
		~CompactSubTask();

		// Memory request for this task
		size_t getRequiredMemory();
		
		// Function to get all the read operations necessary for this task
		void run_read_operations();
		
		// Function to get the ProcessManagerItem to run
        void run_process();
		
	};	
	
}

#endif
