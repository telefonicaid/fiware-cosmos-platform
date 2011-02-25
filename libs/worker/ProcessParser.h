
#ifndef _H_PROCESS_PARSER
#define _H_PROCESS_PARSER

#include "ProcessBase.h"		// ss::ProcessBase

namespace ss {
	
	class WorkerTask;
	class ParserSubTask;
	
	// Individual item of a generator
	
	class ProcessParser : public ProcessBase
	{
		
	public:
		
		ParserSubTask *parserSubTask;
		std::string operation_name;
		Operation * operation;

		char *data;
		size_t size;
		
		// Constructor
		ProcessParser( ParserSubTask *parserSubTask );
		
		// Function to generate key-values
		void generateKeyValues( KVWriter *writer );
		
	};
	
	
}

#endif