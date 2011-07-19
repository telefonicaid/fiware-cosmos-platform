
#ifndef _H_PROCESS_GENERATOR
#define _H_PROCESS_GENERATOR

#include "ProcessBase.h"		// samson::ProcessBase

namespace samson {

	class WorkerTask;
	
	// Individual item of a generator
	
	class ProcessGenerator : public ProcessBase
	{
	public:
		
		std::string generator;		// Generator we will use

        int num_operation;
        int num_operations;
        
		// Constructor
		ProcessGenerator( WorkerTask *task , int num_operation , int num_operations );
		
		// Function to generate key-values
		void generateKeyValues( KVWriter *writer );
		
	};
	
	
}

#endif

