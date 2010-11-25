
#include "OperationFramework.h"		// own interface
#include "samson/Operation.h"		// ss::Operation
#include "ProcessWriter.h"			// ss::ProcessWriter
#include "MemoryManager.h"		// ss::SharedMemoryItem
#include "ProcessWriter.h"		// ss::ProcessWriter

namespace ss {

	
	OperationFramework::OperationFramework( network::ProcessMessage _m )
	{
		// save the message
		m = _m;
		
		// Create the ProcessWriter
		pw = new ProcessWriter( m.output_shm() , m.num_outputs() , m.num_servers() );

	}
	

	
	
}

