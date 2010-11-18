
#include "ProcessAssistantFake.h"				// own interface
#include "SamsonWorker.h"						// ss::SamsonWorker
#include "ProcessAssistantOperationFramework.h"	// ss::ProcessAssistantOperationFramework
#include "ProcessOperationFramework.h"			// ss:ProcessOperationFramework

namespace ss
{
	void* runProcessAssistantFake(void* p)
	{
		ProcessAssistantFake* pa =( ProcessAssistantFake*)p;
		pa->run();
		return NULL;
	}
	
	
	void ProcessAssistantFake::run()
	{
		while( true )
		{
			setStatus( "Asking for the next task" );
			item =  worker->taskManager.getNextItemToProcess();
			setStatus( "Running..." + item->getStatus() );
			
			// Create the Framework to run the operation from the ProcessAssitant side
			Operation * op = worker->modulesManager.getOperation( item->operation );

			assert( op );
			
			framework = new ProcessAssistantOperationFramework(this ,op, core , worker->network->getNumWorkers() );
			framework->setup();
			
			framework2 = new ProcessOperationFramework(this ,op, core , worker->network->getNumWorkers() );
			framework2->setup();
			framework2->run();

			// Flush output
			framework->flushOutput();
			
			delete framework;
			delete framework2;
			
			worker->taskManager.finishItem(item, false, "");
			
			
		}
	}
	
	char* ProcessAssistantFake::passCommand(const char* command)
	{
		framework->flushOutput();
		return NULL;
	}
	
	std::string ProcessAssistantFake::getOutputQueue( int i )
	{
		return item->outputs[i];
	}
	
	size_t ProcessAssistantFake::getTaskId()
	{
		return item->task_id;
	}

	
	
}