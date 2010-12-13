
#include "WorkerTaskItemParser.h"		// Own interface
#include "ProcessAssistant.h"			// ss::ProcessAssistant
#include "WorkerTask.h"					// ss::WorkerTask
#include "SamsonSetup.h"				// ss::SamsonSetup
#include "FileManagerReadItem.h"
#include "FileManagerWriteItem.h"

namespace ss {

#pragma mark WorkerTaskItemParser
	
	void WorkerTaskItemParser::setupInputs()
	{
		SharedMemoryItem * smi = MemoryManager::shared()->getSharedMemory( shm_input );
		size = FileManagerReadItem::sizeOfFile( fileName );
		
		FileManagerReadItem *item = new FileManagerReadItem( fileName , 0 , size , smi->data , NULL );
		addInputFiles( item );
		
		// The delegate is TaskManager: it received notifications form the FileManager and unblock items
	}
	
	void WorkerTaskItemParser::run( ProcessAssistant *pa )
	{
		
		// Create the Framework to run the operation from the ProcessAssitant side
		Operation * op = pa->taskManager->worker->modulesManager.getOperation( operation );
		assert( op );
		
		network::ProcessMessage p;
		p.set_code( network::ProcessMessage::run );
		p.set_operation( operation );
		p.set_num_servers( pa->taskManager->worker->workers );
		p.set_num_inputs( op->getNumInputs() );
		p.set_num_outputs( op->getNumOutputs() );
		p.set_output_shm( pa->output_shm );					// Set the output shared memory buffer
		p.set_input_size( size );								// Set the size of input for parsing
		p.mutable_worker_task()->CopyFrom( workerTask );
		p.set_input_shm( shm_input );						// Set the input shared memory buffer
		
		if( op->getType() == Operation::parserOut)
		{
			p.set_output_txt(true);
			p.set_output_kvs(false);
		}
		else
		{
			p.set_output_txt(false);
			p.set_output_kvs(true);
		}
		
		
		// Create the framework here at the process assistant side
		framework = new ProcessAssistantOperationFramework(pa , p );
		
		// Blocking command until the Process has complete the job
		pa->runCommand( p );	
		
		// Flush output
		framework->flushOutput(this);
		
		delete framework;
	}	
	

	
	
	
	
}