



#ifndef _H_WORKER_TASK_ITEM_PARSER
#define _H_WORKER_TASK_ITEM_PARSER

#include "WorkerTaskItemWithOutput.h"	// ss::WorkerTaskItemWithOutput
#include "ProcessAssistant.h"			// ss::ProcessAssistant
#include "SamsonWorker.h"				// ss::SamsonWorker
#include "ProcessAssistantOperationFramework.h"	// ss::ProcessAssistantOperationFramework

namespace ss {
	
	// Individual item of a parser

	class ProcessAssistant;
	
	class WorkerTaskItemOperation : public WorkerTaskItemWithOutput
	{
		
	public:
		
		std::string operation;		// operation name
		std::string fileName;		// Input file to parse
		ProcessAssistantOperationFramework * framework;
		
		WorkerTaskItemOperation(  std::string _fileName , const network::WorkerTask &task ) : WorkerTaskItemWithOutput( task )
		{
			operation = task.operation();
			fileName = _fileName;
			
			// Request this file to be loaded by FileManager
			readItemVector = new FileManagerReadItemVector();
			readItemVector->addItem( FileManagerReadItem( fileName ) );
			
			setup();
		}
		
		~WorkerTaskItemOperation()
		{
			delete readItemVector;
		}
		
		void run( ProcessAssistant *pa );

		virtual void receiveCommand( ProcessAssistant *pa , network::ProcessMessage p )
		{
			framework->flushOutput(this);
		}
		
	};

}

#endif