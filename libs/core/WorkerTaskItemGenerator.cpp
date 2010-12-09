
#include "WorkerTaskItemGenerator.h"
#include "WorkerTask.h"

namespace ss {

	void WorkerTaskItemGenerator::run( ProcessAssistant *pa )
	{
		
		// Create the Framework to run the operation from the ProcessAssitant side
		Operation * op = pa->taskManager->worker->modulesManager.getOperation( generator );
		assert( op );
		
		
		network::ProcessMessage p;
		p.set_code( network::ProcessMessage::run );
		p.set_operation( generator );
		p.set_num_servers( pa->taskManager->worker->workers );
		p.set_num_inputs( op->getNumInputs() );
		p.set_num_outputs( op->getNumOutputs() );
		p.set_output_shm( pa->output_shm );
		p.mutable_worker_task()->CopyFrom( workerTask );
		p.set_output_txt(false);
		p.set_output_kvs(true);

		
		// Create the framework here at the process assistant side
		framework = new ProcessAssistantOperationFramework(pa , p );
		
		// Blocking command until the Process has complete the job
		network::ProcessMessage received_packet = pa->runCommand( p );	
		
		
		if( received_packet.code() == network::ProcessMessage::error )
			setError("Some error during execution");

		if( received_packet.code() == network::ProcessMessage::crash )
			setError("Process crashed");
				
		// Flush output
		framework->flushOutput(this);
		
		delete framework;
	}

}
