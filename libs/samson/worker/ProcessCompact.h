
#ifndef _H_PROCESS_COMPACT
#define _H_PROCESS_OPERATION

#include "ProcessBase.h"			// samson::ProcessBase
#include "engine/ProcessItem.h"            // engine::ProcessItem

namespace samson
{
	
	class WorkerTaskManager;
	class CompactSubTask;
	
	class ProcessCompact : public engine::ProcessItem
	{
		CompactSubTask * compactSubTask;
		
		// Information we need to report new file
		std::string fileName;
		std::string queue;
		
		WorkerTaskManager *tm;
		size_t task_id;
		
	public:
		
		ProcessCompact( CompactSubTask * _compactSubTask );
		~ProcessCompact();
		
		void run();
		
		
	};
	
	
}

#endif
