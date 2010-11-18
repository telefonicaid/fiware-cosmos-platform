
#include "OperationFramework.h"		// own interface
#include "samson/Operation.h"		// ss::Operation
#include "ProcessWriter.h"			// ss::ProcessWriter
#include "MemoryManager.h"		// ss::SharedMemoryItem
#include "ProcessWriter.h"		// ss::ProcessWriter

namespace ss {

	
	OperationFramework::OperationFramework( int processId , int _num_servers )
	{
		process = NULL;
		processAssistant = NULL;
		
		sm = MemoryManager::shared()->getSharedMemory( processId );
		num_servers = _num_servers;
		
		// Header pointing to the beginging of the shared memory area
		header = (OperationFrameworkHeader*)sm->data;
		
	}
	
	
	void OperationFramework::createProcessWriter()
	{
		// Writer from header
		pw = new ProcessWriter(sm->data + header->output.offset , header->output.size , operation->getNumOutputs() , num_servers );
	}

	void OperationFramework::setOperation( Operation *_operation )
	{
		operation = _operation;
	}
	
	
}

