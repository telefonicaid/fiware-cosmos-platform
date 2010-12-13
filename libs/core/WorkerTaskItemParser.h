
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
		size_t size;				// Size of this file
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
	
	

	
	
}

#endif