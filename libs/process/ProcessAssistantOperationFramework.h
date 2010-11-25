#ifndef _H_PROCESS_ASSISTANT_OPERATION_FRAMEWORK
#define _H_PROCESS_ASSISTANT_OPERATION_FRAMEWORK

#include "OperationFramework.h"		// ss::OperationFramework
#include "samson/Operation.h"		// ss::Operation

namespace ss {
	
	class WorkerTaskItemWithOutput;
	
	class ProcessAssistantOperationFramework : public OperationFramework
	{
		ProcessAssistant *processAssistant;
		
	public:
		
		ProcessAssistantOperationFramework(ProcessAssistant *_processAssistant , network::ProcessMessage m  );
		
		/**
		 Function used to add input files for each one of the inputs ( prior to setup call)
		 */
		void addInputFiles();

		
		void flushOutput(WorkerTaskItemWithOutput *item);
		
	};
	
}

#endif
