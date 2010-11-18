#ifndef _H_PROCESS_ASSISTANT_OPERATION_FRAMEWORK
#define _H_PROCESS_ASSISTANT_OPERATION_FRAMEWORK

#include "OperationFramework.h"		// ss::OperationFramework
#include "samson/Operation.h"		// ss::Operation

namespace ss {
	
	class ProcessAssistantOperationFramework : public OperationFramework
	{
		
	public:
		
		ProcessAssistantOperationFramework(ProcessAssistantInterface *_processAssistant ,Operation *_operation, int processId , int _num_servers  );
		
		/**
		 Function used to add input files for each one of the inputs ( prior to setup call)
		 */
		void addInputFiles();

		void setup();

		void flushOutput();
		
	};
	
}

#endif
