
#include "WorkerTaskItemParser.h"		// Own interface
#include "ProcessAssistant.h"			// ss::ProcessAssistant
#include "WorkerTask.h"					// ss::WorkerTask

namespace ss {

	void WorkerTaskItemOperation::run( ProcessAssistant *pa )
	{
		
		// Create the Framework to run the operation from the ProcessAssitant side
		Operation * op = pa->worker->modulesManager.getOperation( operation );
		assert( op );
		
		network::ProcessMessage p;
		p.set_code( network::ProcessMessage::run );
		p.set_operation( operation );
		p.set_num_servers( pa->worker->workers );
		p.set_num_inputs( op->getNumInputs() );
		p.set_num_outputs( op->getNumOutputs() );
		
		//p.set_input_size( XXX );			// For Parse operation
		
		p.set_output_shm( pa->output_shm );					// Set the output shared memory buffer
		
		p.set_input_shm( shm_input );						// Set the input shared memory buffer
		
		p.mutable_environment()->CopyFrom( task->environment );
		
		// Create the framework here at the process assistant side
		framework = new ProcessAssistantOperationFramework(pa , p );
		
		// Blocking command until the Process has complete the job
		pa->runCommand( p );	
		
		// Flush output
		framework->flushOutput(this);
		
		delete framework;
	}
	
	
	void WorkerTaskItemOperation::setupInputs()
	{
		SharedMemoryItem * smi = MemoryManager::shared()->getSharedMemory( shm_input );
		size_t size = FileManagerReadItem::sizeOfFile( fileName );
		
		FileManagerReadItem *item = new FileManagerReadItem( fileName , 0 , size , smi->data , task->taskManager );
		addInputFiles( item );
		
		// The delegate is TaskManager: it received notifications form the FileManager and unblock items
		
	}
	

	
	
}