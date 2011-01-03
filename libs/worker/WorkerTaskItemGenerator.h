


#ifndef _H_WORKER_TASK_ITEM_GENERATOR
#define _H_WORKER_TASK_ITEM_GENERATOR

#include "WorkerTaskItem.h"	// ss::WorkerTaskItemWithOutput
#include "ProcessAssistant.h"			// ss::ProcessAssistant
#include "SamsonWorker.h"				// ss::SamsonWorker
#include "ProcessAssistantOperationFramework.h"	// ss::ProcessAssistantOperationFramework

namespace ss {
		
	class ProcessAssistantOperationFramework;

	// Individual item of a generator
	
	class WorkerTaskItemGenerator : public WorkerTaskItem
	{
	public:
		
		std::string generator;
		ProcessAssistantOperationFramework * framework;
		
		WorkerTaskItemGenerator( const network::WorkerTask &task ) : WorkerTaskItem( task )
		{
			generator = task.operation();
			framework = NULL;

			//state = ready_to_run;	// No input is never necessary for this operation
		}
		
		virtual void run( ProcessAssistant *pa );
		
		virtual void receiveCommand( ProcessAssistant *pa , network::ProcessMessage p )
		{
			assert( framework );
			framework->flushOutput(this);
		}
		
		
	};
}
#endif