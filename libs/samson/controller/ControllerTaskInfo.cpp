
#include "ControllerTaskInfo.h"			// Own interface
#include "samson/module/Operation.h"			// samson::Operation
#include "Job.h"						// samson::Job
#include <iostream>

namespace samson {

	
	ControllerTaskInfo::ControllerTaskInfo( Job *_job , Operation *_operation  , au::CommandLine *cmdLine )
	{
		job = _job;
		operation = _operation;
		error = false;
		
		// Operation name
		operation_name = cmdLine->get_argument(0);
		

		int pos = 1;
		for (int i = 0 ; i < operation->getNumInputs() ;i++)
			inputs.push_back( cmdLine->get_argument( pos++) );
		
		for (int i = 0 ; i < operation->getNumOutputs() ;i++)
			outputs.push_back( cmdLine->get_argument( pos++) );
		
		
	}
	
	ControllerTaskInfo::~ControllerTaskInfo()
	{

		for ( size_t i = 0 ; i < input_queues.size() ; i++)
			delete input_queues[i];		
		
		for ( size_t i = 0 ; i < output_queues.size() ; i++)
			delete output_queues[i];		
	}
	
	void ControllerTaskInfo::setError( std::string _error_message)
	{
		error = true;
		error_message = _error_message;
	}

}
