#ifndef _H_PROCESS_ASSISTANT_FAKE
#define _H_PROCESS_ASSISTANT_FAKE

#include "ProcessAssistantInterface.h"		// ss::ProcessAssistantInterface
#include "ProcessInterface.h"				// ss::ProcessInterface

namespace ss {

	
	class ProcessAssistantOperationFramework;
	class ProcessOperationFramework;
	class WorkerTaskItem;
	/**
	 Fake replica to debug in the same thread
	 */
	
	void* runProcessAssistantFake(void* p);
	
	class ProcessAssistantFake : public ProcessAssistantInterface , public ProcessInterface
	{

		ProcessAssistantOperationFramework * framework;
		ProcessOperationFramework * framework2;
		
		WorkerTaskItem *item;	// Current task
		
		pthread_t t;
	public:
		
		ProcessAssistantFake( int coreNo , SamsonWorker*worker ) : ProcessAssistantInterface(coreNo, worker)
		{
			item = NULL;
			pthread_create(&t, NULL, runProcessAssistantFake, this);
		}
		
		void run();		

		
		// Pass command of the process
		virtual char* passCommand(const char* command);
		
		
		// get the status of this element
		virtual std::string getStatus()
		{
			return "Fake ProcessAssistant";
		}
		
		
		std::string getOutputQueue( int i );
		
		virtual size_t getTaskId();

		
	};	
}

#endif
