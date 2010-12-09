
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
		
		// Create the ProcessWriter or ProcessTxtWriter
		
		if( m.output_kvs() )
		{
			pw = new ProcessWriter( m.output_shm() , m.num_outputs() , m.num_servers() );
			assert( !m.output_txt() );
		}
		else
			pw = NULL;

		
		if( m.output_txt() )
		{
			ptw = new ProcessTXTWriter( m.output_shm() , m.num_outputs() , m.num_servers() );
			assert( !m.output_kvs() );
		}
		else
			ptw = NULL;
		
	}
	

	
	
}

