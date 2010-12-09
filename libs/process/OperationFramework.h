#ifndef _H_OPERATION_FRAMEWORK
#define _H_OPERATION_FRAMEWORK

#include <cstring>
#include <string>
#include "samson.pb.h"

namespace ss {

	class Process;
	class ProcessAssistant;
	class ProcessWriter;
	class Operation;
	class SharedMemoryItem;
	class ProcessTXTWriter;
	
	/**
	 Full class for a particualr operation process
	 */
	
	
	class OperationFramework
	{
		
	protected:
		
		network::ProcessMessage m;	// Message that originated this operation
		
		ProcessWriter *pw;			// Process writer to emit key-values
		ProcessTXTWriter *ptw;		// Process writer to emit txt
		
		OperationFramework( network::ProcessMessage m );		
		
	};
	
	
}

#endif
