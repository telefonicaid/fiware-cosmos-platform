
#ifndef _H_PROCESS_PARSER
#define _H_PROCESS_PARSER

#include "ProcessBase.h"		// samson::ProcessBase

namespace samson {
	
	class WorkerTask;
	class ParserSubTask;
	
	// Individual item of a generator
	
	class ProcessParser : public ProcessBase
	{
		
	public:
		
		ParserSubTask *parserSubTask;
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