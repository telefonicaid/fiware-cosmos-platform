


#ifndef _H_WORKER_TASK_ITEM_GENERATOR
#define _H_WORKER_TASK_ITEM_GENERATOR

#include "WorkerTaskItemWithOutput.h"	// ss::WorkerTaskItemWithOutput
#include "ProcessAssistant.h"			// ss::ProcessAssistant
#include "SamsonWorker.h"				// ss::SamsonWorker
#include "ProcessAssistantOperationFramework.h"	// ss::ProcessAssistantOperationFramework

namespace ss {
		
	class ProcessAssistantOperationFramework;

	// Individual item of a generator
	
	class WorkerTaskItemGenerator : public WorkerTaskItemWithOutput
	{
	public:
		
		std::string generator;
		ProcessAssistantOperationFramework * framework;
		
		WorkerTaskItemGenerator( size_t _task_id , size_t _item_id , const network::WorkerTask &task ) 
		: WorkerTaskItemWithOutput( _task_id ,_item_id , WorkerTaskItem::generator ,  task)
		{
			generator = task.operation();
			framework = NULL;
			
			setup();
		}
		
		virtual void run( ProcessAssistant *pa )
		{
			
			// Create the Framework to run the operation from the ProcessAssitant side
			Operation * op = pa->worker->modulesManager.getOperation( generator );
			assert( op );

			
			network::ProcessMessage p;
			p.set_code( network::ProcessMessage::run );
			p.set_operation( generator );
			p.set_num_servers( pa->worker->workers );
			p.set_num_inputs( op->getNumInputs() );
			p.set_num_outputs( op->getNumOutputs() );
			p.set_output_shm( pa->output_shm );

			// Create the framework here at the process assistant side
			framework = new ProcessAssistantOperationFramework(pa , p );
			
			// Blocking command until the Process has complete the job
			pa->runCommand( p );	
			
			// Flush output
			framework->flushOutput(this);
			
			delete framework;
		}
		
		virtual void receiveCommand( ProcessAssistant *pa , network::ProcessMessage p )
		{
			assert( framework );
			framework->flushOutput(this);
		}
		
		
	};
}
#endif