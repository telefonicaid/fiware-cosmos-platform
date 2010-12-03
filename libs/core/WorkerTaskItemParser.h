



#ifndef _H_WORKER_TASK_ITEM_PARSER
#define _H_WORKER_TASK_ITEM_PARSER

#include "WorkerTaskItemWithOutput.h"	// ss::WorkerTaskItemWithOutput
#include "ProcessAssistant.h"			// ss::ProcessAssistant
#include "SamsonWorker.h"				// ss::SamsonWorker
#include "ProcessAssistantOperationFramework.h"	// ss::ProcessAssistantOperationFramework

namespace ss {
	
	// Individual item of a parser

	class ProcessAssistant;
	
	class WorkerTaskItemOperation : public WorkerTaskItemWithOutput
	{
		
	public:
		
		std::string operation;		// operation name
		std::string fileName;		// Input file to parse
		ProcessAssistantOperationFramework * framework;
		
		WorkerTaskItemOperation(  std::string _fileName , const network::WorkerTask &task ) : WorkerTaskItemWithOutput( task )
		{
			operation = task.operation();
			fileName = _fileName;

			// This state indicate that we need a shared memory region to run this item
			state = no_memory;

		}
	
		void setupInputs();
		
		void run( ProcessAssistant *pa );

		virtual void receiveCommand( ProcessAssistant *pa , network::ProcessMessage p )
		{
			framework->flushOutput(this);
		}
		
	};
	
	/**
	 Item necessary to organize the reduce operation
	 */
	
	class WorkerTaskItemReduceOrganizer : public WorkerTaskItem
	{
		
		network::WorkerTask workerTask;

		WorkerTask * parentTask;					// Pointer to parent task to add children items when necessary
	
		friend class WorkerTaskItemReduce;
		
	public:
		 
		WorkerTaskItemReduceOrganizer( WorkerTask * _parentTask, const network::WorkerTask & _workerTask );		
		
		
		void setupInputs();

		void run( ProcessAssistant *pa );

		
		virtual void receiveCommand( ProcessAssistant *pa , network::ProcessMessage p )
		{
			// Nothing good if this is called
			assert( false );
		}
		
		
	};
	
	


	class WorkerTaskItemReduce : public WorkerTaskItemWithOutput
	{
		
	public:
		
		ProcessAssistantOperationFramework * framework;
		network::WorkerTask workerTask;
		WorkerTask *parentTask;
		
		int hg_begin;
		int hg_end;
		
		int num_hash_groups;
		
		WorkerTaskItemReduce( int _hg_begin , int _hg_end , WorkerTaskItemReduceOrganizer* reduceOrganizer, const network::WorkerTask &task ) : WorkerTaskItemWithOutput( task )
		{
			// Save all the information
			workerTask = task;

			// Get a pointer to the task
			parentTask = reduceOrganizer->parentTask;
			
			// Starting and ending hash-group
			hg_begin = _hg_begin;
			hg_end = _hg_end;
			
			num_hash_groups = hg_end - hg_begin;
			
			// This state indicate that we need a shared memory region to run this item
			state = no_memory;
			
		}
		
		~WorkerTaskItemReduce()
		{
		}
		
		void setupInputs();
		void run( ProcessAssistant *pa );
		
		virtual void receiveCommand( ProcessAssistant *pa , network::ProcessMessage p )
		{
			framework->flushOutput(this);
		}
		
	};
	
	
}

#endif