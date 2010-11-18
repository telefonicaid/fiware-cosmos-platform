
#import "ProcessAssistantOperationFramework.h"		// Own interface
#import "assert.h"									// assert(.)
#import "MemoryManager.h"							// ss::SharedMemoryItem
#import "ProcessWriter.h"							// ss::ProcessWriter

namespace ss {


	ProcessAssistantOperationFramework::ProcessAssistantOperationFramework(ProcessAssistantInterface *_processAssistant ,Operation *_operation, int processId , int _num_servers  ) : OperationFramework( processId , _num_servers )
	{
		processAssistant = _processAssistant;
		operation = _operation;
		
		assert( operation );
		assert( processAssistant );
	}

	/**
	 Function used to add input files for each one of the inputs ( prior to setup call)
	 */
	void ProcessAssistantOperationFramework::addInputFiles()
	{
	}

	void ProcessAssistantOperationFramework::setup()
	{
		assert( operation );
		
		size_t offset = sizeof( OperationFrameworkHeader );	// Skip the header
		
		// Now we only consider generator elements ( so no inputs at all)
		header->num_inputs = 0;		
		assert( operation->getNumInputs() == 0);
		
		// The output takes the rest of the space
		header->output.offset	= offset;
		header->output.size		= sm->size -  offset;
		
		// Writer from header
		createProcessWriter();	// Create the Process writter with the information in the header
		pw->setProcessAssistant( processAssistant );
	}

	void ProcessAssistantOperationFramework::flushOutput()
	{
		// Flush the pw buffer to emit the key-values in the output buffer
		pw->FlushBuffer();
	}
}

