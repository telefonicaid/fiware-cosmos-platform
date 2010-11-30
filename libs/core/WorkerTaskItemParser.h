



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

}

#endif