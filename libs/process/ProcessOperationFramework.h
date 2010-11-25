#ifndef _H_PROCESS_OPERATION_FRAMEWORK
#define _H_PROCESS_OPERATION_FRAMEWORK

#include "OperationFramework.h"				// Own interface
#include "ProcessWriter.h"					// ss::ProcessWriter
#include "samson.pb.h"						// ss::network::,,,

namespace ss {

	class Process;
	class WorkerTaskItemWithOutput;
	
	class ProcessOperationFramework : public OperationFramework
	{
		
		Process *process;
		Operation *operation;
		
	public:
		
		ProcessOperationFramework( Process *_process , network::ProcessMessage m  ) : OperationFramework( m )
		{
			
			process = _process;
			assert( process );
			
			// We still have no idea about the operation to run
			operation = process->modulesManager.getOperation( m.operation() );
			assert( operation );
			
			// Set the process to report "full buffer messages"
			pw->setProcess( process );	

		}
		
		void flushOutput( WorkerTaskItemWithOutput *taskItem);
		
		
		void run()
		{
			// Run the operation
			assert( operation );
			
			switch (operation->getType()) {
				case Operation::generator:
				{
					// Run the generator over the ProcessWriter to emit all key-values
					Generator *generator = (Generator*) operation->getInstance();
					generator->run( pw );
					break;
				}
				case Operation::parser:
				{
					// Run the generator over the ProcessWriter to emit all key-values
					Parser *parser = (Parser*) operation->getInstance();
					
					SharedMemoryItem*item =  MemoryManager::shared()->getSharedMemory( m.input_shm() );
					
					parser->run( item->data , m.input_size() ,  pw );
					break;
				}
					
				default:
					assert( false );
					break;
			}
			
		}
	};	
}

#endif
