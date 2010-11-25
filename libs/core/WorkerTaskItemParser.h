



#ifndef _H_WORKER_TASK_ITEM_PARSER
#define _H_WORKER_TASK_ITEM_PARSER

#include "WorkerTaskItemWithOutput.h"	// ss::WorkerTaskItemWithOutput
#include "ProcessAssistant.h"			// ss::ProcessAssistant
#include "SamsonWorker.h"				// ss::SamsonWorker
#include "ProcessAssistantOperationFramework.h"	// ss::ProcessAssistantOperationFramework

namespace ss {
	
	// Individual item of a parser

	class ProcessAssistant;
	
	class WorkerTaskItemParser : public WorkerTaskItemWithOutput
	{
		
	public:
		
		std::string parser;		// Parse operation
		std::string fileName;	// Input file to parse
		ProcessAssistantOperationFramework * framework;
		
		
		WorkerTaskItemParser( size_t _task_id , size_t _item_id , std::string _fileName, const network::WorkerTask &task ) 
		: WorkerTaskItemWithOutput( _task_id ,_item_id , WorkerTaskItem::generator, task)
		{
			parser = task.operation();
			fileName = _fileName;
			
			// Request this file to be loaded by FileManager
			readItemVector = new FileManagerReadItemVector();
			readItemVector->addItem( FileManagerReadItem( fileName ) );
			
			setup();
		}
		
		~WorkerTaskItemParser()
		{
			delete readItemVector;
		}
		
		void run( ProcessAssistant *pa )
		{
			
			// Create the Framework to run the operation from the ProcessAssitant side
			Operation * op = pa->worker->modulesManager.getOperation( parser );
			assert( op );
			
			network::ProcessMessage p;
			p.set_code( network::ProcessMessage::run );
			p.set_operation( parser );
			p.set_num_servers( pa->worker->workers );
			p.set_num_inputs( op->getNumInputs() );
			p.set_num_outputs( op->getNumOutputs() );

			p.set_input_size( readItemVector->size );	// Total size for input
			
			p.set_output_shm( pa->output_shm );

			p.set_input_shm( shm_input );	// Set the input shared memory 
							
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
			//framework->flushOutput(this);
		}
		
	};

}

#endif