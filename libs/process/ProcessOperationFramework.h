#ifndef _H_PROCESS_OPERATION_FRAMEWORK
#define _H_PROCESS_OPERATION_FRAMEWORK

#include "OperationFramework.h"				// Own interface
#include "ProcessWriter.h"					// ss::ProcessWriter

namespace ss {

	class ProcessInterface;
	
	class ProcessOperationFramework : public OperationFramework
	{
	public:
		
		ProcessOperationFramework( ProcessInterface *_process, Operation *_operation, int processId , int _num_servers  ) : OperationFramework( processId , _num_servers )
		{
			process = _process;
			operation = _operation;
			
			assert( process );
			assert( operation );
		}
		
		void setup()
		{
			assert( operation );
			
			createProcessWriter();		// Create the Process writter with the information in the header
			pw->setProcess( process );	// Set the process to report "full buffer messages"
		}
		
		void flushOutput()
		{
			pw->FlushBuffer();
		}
		
		
		void run()
		{
			// Run the operation
			assert( operation );
			assert( operation->getNumInputs()==0);
			
			switch (operation->getType()) {
				case Operation::generator:
				{
					// Run the generator over the ProcessWriter to emit all key-values
					Generator *generator = (Generator*) operation->getInstance();
					generator->run( pw );
					break;
				}
				default:
					break;
			}
			
		}
	};	
}

#endif
