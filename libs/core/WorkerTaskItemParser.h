



#ifndef _H_WORKER_TASK_ITEM_PARSER
#define _H_WORKER_TASK_ITEM_PARSER

#include "WorkerTaskItem.h"	// ss::WorkerTaskItemWithOutput
#include "ProcessAssistant.h"			// ss::ProcessAssistant
#include "SamsonWorker.h"				// ss::SamsonWorker
#include "ProcessAssistantOperationFramework.h"	// ss::ProcessAssistantOperationFramework

namespace ss {
	
	// Individual item of a parser

	class ProcessAssistant;
	
	
	class WorkerTaskItemParser : public WorkerTaskItem
	{
		
	public:
		
		std::string operation;		// operation name
		std::string fileName;		// Input file to parse
		ProcessAssistantOperationFramework * framework;
		
		WorkerTaskItemParser(  std::string _fileName , const network::WorkerTask &task ) : WorkerTaskItem( task )
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
	
	class WorkerTaskItemOrganizer : public WorkerTaskItem
	{
		friend class WorkerTaskItemOperation;
		
	public:
		 
		WorkerTaskItemOrganizer( const network::WorkerTask & _workerTask );		
		
		void setupInputs();

		void run( ProcessAssistant *pa );

		
		virtual void receiveCommand( ProcessAssistant *pa , network::ProcessMessage p )
		{
			// Nothing good if this is called
			assert( false );
		}
		
	};

	class WorkerTaskItemOperation : public WorkerTaskItem
	{
		
	public:
		
		ProcessAssistantOperationFramework * framework;
		
		int hg_begin;
		int hg_end;
		int num_hash_groups;
		
		WorkerTaskItemOperation( int _hg_begin , int _hg_end , WorkerTaskItemOrganizer* reduceOrganizer, const network::WorkerTask &task ) : WorkerTaskItem( task )
		{
			// Starting and ending hash-group
			hg_begin = _hg_begin;
			hg_end = _hg_end;
			num_hash_groups = hg_end - hg_begin;
			
			// This state indicate that we need a shared memory region to run this item
			state = no_memory;
		}
		
		~WorkerTaskItemOperation()
		{
		}
		
		void setupInputs();
		void run( ProcessAssistant *pa );

		
		virtual void receiveCommand( ProcessAssistant *pa , network::ProcessMessage p )
		{
			assert( framework );
			framework->flushOutput(this);
		}
		
	};
	
	
}

#endif