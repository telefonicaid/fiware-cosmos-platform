#ifndef _H_WORKER_TASK_ITEM_OPERATION
#define _H_WORKER_TASK_ITEM_OPERATION

#include "WorkerTaskItem.h"							// ss::WorkerTaskItemWithOutput
#include "ProcessAssistant.h"						// ss::ProcessAssistant
#include "SamsonWorker.h"							// ss::SamsonWorker
#include "ProcessAssistantOperationFramework.h"		// ss::ProcessAssistantOperationFramework

namespace ss {
	
	class WorkerTaskItemOrganizer;
	
	class WorkerTaskItemOperation : public WorkerTaskItem
	{
		
	public:
		
		ProcessAssistantOperationFramework * framework;
		
		int hg_begin;
		int hg_end;
		int num_hash_groups;
		
		WorkerTaskItemOperation( int _hg_begin , int _hg_end , WorkerTaskItemOrganizer* reduceOrganizer, const network::WorkerTask &task );
		
		~WorkerTaskItemOperation();
		
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