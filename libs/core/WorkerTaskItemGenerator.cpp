
#include "WorkerTaskItemGenerator.h"
#include "WorkerTask.h"

namespace ss {

	void WorkerTaskItemGenerator::run( ProcessAssistant *pa )
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

		// Put environment information in the message
		p.mutable_environment()->CopyFrom( task->environment );
		
		// Create the framework here at the process assistant side
		framework = new ProcessAssistantOperationFramework(pa , p );
		
		// Blocking command until the Process has complete the job
		pa->runCommand( p );	
		
		// Flush output
		framework->flushOutput(this);
		
		delete framework;
	}

}
