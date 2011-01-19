
#ifndef _H_PROCESS_OPERATION
#define _H_PROCESS_OPERATION

#include "ProcessBase.h"		// ss::ProcessBase

namespace ss {
	
	class WorkerTask;
	class OperationSubTask;
	
	// Individual item of a generator
	
	class ProcessOperation : public ProcessBase
	{
		
	public:

		OperationSubTask *operationSubTask;
		std::string operation_name;
		Operation * operation;
		char *data;
		
		// Constructor
		ProcessOperation( OperationSubTask *operationSubTask );
		
		// Function to generate key-values or txt
		void generateKeyValues( KVWriter *writer );
		void generateTXT( TXTWriter *writer );
		
	private:
		
		void runMap( KVWriter *writer );
		void runReduce( KVWriter *writer );
		void runParserOut(TXTWriter *writer );
		
	};
	
	
}

#endif
