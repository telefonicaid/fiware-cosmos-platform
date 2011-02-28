

#include "ProcessGenerator.h"		// Own interface
#include "ModulesManager.h"			// ss::ModulesManager
#include "WorkerTask.h"				// ss::WorkerTask

namespace ss {
	
	ProcessGenerator::ProcessGenerator( WorkerTask *task ) : ProcessBase( task , ProcessBase::key_value  )
	{
		// Name of the generator
		generator = task->workerTask.operation();	
		
		// Description with the name of the generator
		setStatus( std::string("G:") + generator );
	}
	
	
	void ProcessGenerator::generateKeyValues( KVWriter *writer )
	{
		// Create the Framework to run the operation from the ProcessAssitant side
		ModulesManager *modulesManager = ModulesManager::shared();
		
		Operation * op = modulesManager->getOperation( generator );
		assert( op );
		
		
		// Get a generator instance
		
		Generator * generator  = (Generator*) op->getInstance();
		generator->environment = &environment;			// To be able to access environment
		generator->tracer = this;						// To be able to send traces
		generator->operationController = this;
		
		generator->run( writer );
		
		delete generator;
		
	}

}