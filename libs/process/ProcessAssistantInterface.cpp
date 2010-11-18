
#include "ProcessAssistantInterface.h"		// Own interface
#include "SamsonWorker.h"					// ss::SamsonWorker

namespace ss
{
	ProcessAssistantInterface::ProcessAssistantInterface( int _core , SamsonWorker* _worker )
	{
		status = "no status defined...";
		core = _core;
		worker = _worker;
	}
	
	
}