

#include "ProcessGenerator.h"		// Own interface
#include "samson/module/ModulesManager.h"			// samson::ModulesManager
#include "WorkerTask.h"				// samson::WorkerTask

namespace samson {
	
	ProcessGenerator::ProcessGenerator( WorkerTask *task , int _num_operation , int _num_operations ) : ProcessBase( task , ProcessBase::key_value  )
	{
		// Name of the generator
		generator = task->workerTask.operation();	
		
        // Operation number and total number of operations
        num_operation = _num_operation;
        num_operations = _num_operations;
        
		// Description with the name of the generator
		operation_name =  std::string("G:") + generator;
	}
	
	
	void ProcessGenerator::generateKeyValues( KVWriter *writer )
	{
		// Create the Framework to run the operation from the ProcessAssitant side
		ModulesManager *modulesManager = ModulesManager::shared();
		
		Operation * op = modulesManager->getOperation( generator );
		if( !op )
			LM_X(1,("Internal error: Unknown operation at ProcessGenerator"));
		
		
		// Get a generator instance
		
		Generator * generator  = (Generator*) op->getInstance();
		generator->environment = &environment;			// To be able to access environment
		generator->tracer = this;						// To be able to send traces
		generator->operationController = this;
        
		generator->init(writer);
        
        generator->setup( worker , num_workers , num_operation , num_operations );
        
		generator->run( writer );
		generator->finish(writer);
		
		delete generator;
		
	}

}
