
#ifndef _H_WORKER_TASK_ITEM_ORGANIZER
#define _H_WORKER_TASK_ITEM_ORGANIZER

#include "WorkerTaskItem.h"							// ss::WorkerTaskItemWithOutput
#include "ProcessAssistant.h"						// ss::ProcessAssistant
#include "SamsonWorker.h"							// ss::SamsonWorker
#include "ProcessAssistantOperationFramework.h"		// ss::ProcessAssistantOperationFramework

namespace ss {	
	
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
	
}

#endif