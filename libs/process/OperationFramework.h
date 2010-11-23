#ifndef _H_OPERATION_FRAMEWORK
#define _H_OPERATION_FRAMEWORK

#include <cstring>

namespace ss {

	class ProcessInterface;
	class ProcessAssistantInterface;
	class ProcessWriter;
	class Operation;
	class SharedMemoryItem;
	
	
	/**
	 Full class for a particualr operation process
	 */
	
	struct OperationFrameworkHeaderItem
	{
		size_t offset;
		size_t size;
	};
	
	struct OperationFrameworkHeader
	{
		char num_inputs;
		OperationFrameworkHeaderItem inputs[255];	// Part of the shared memory vector for the input
		OperationFrameworkHeaderItem output;		// Part of the shared memory vector for the output ( unique for all servers and 
	};
	
	
	class OperationFramework
	{
		
	protected:
		
		// Pointer to the parent ( one of them )
		ProcessInterface *process;
		ProcessAssistantInterface *processAssistant;
		
		Operation *operation;
		int num_servers;
		
		SharedMemoryItem* sm;	// Shared memory used
		ProcessWriter *pw;		// Process writer to emit key-values
		
		OperationFrameworkHeader *header;
		
		OperationFramework( int processId , int _num_servers );		
		void setOperation( Operation *_operation );
		
		void createProcessWriter();
		
	};
	
	
}

#endif
