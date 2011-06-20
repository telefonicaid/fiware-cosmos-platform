
#include "ControllerTaskInfo.h"			// Own interface

#include "samson/module/Operation.h"			// samson::Operation
#include "samson/module/ModulesManager.h"       // samson::ModulesManager

#include "Job.h"						// samson::Job
#include <iostream>

namespace samson {

	
	ControllerTaskInfo::ControllerTaskInfo( Job *_job , std::string _command  )
	{
		job = _job;
        environment.set("command" , _command );
		
		
        // Process incomming command
        au::CommandLine cmdLine;
        cmdLine.set_flag_boolean("create");
        cmdLine.set_flag_boolean("clear");
        cmdLine.set_flag_boolean("c");

        cmdLine.parse( _command );
        
        flag_create = cmdLine.get_flag_bool("create") || cmdLine.get_flag_bool("c"); 
        flag_clear  = cmdLine.get_flag_bool("clear");

        if( cmdLine.get_num_arguments() == 0)
        {
            error.set("Not valid operation name");
            return;
        }
        
		// Operation name
		operation_name = cmdLine.get_argument(0);

        // get the operation
        operation = ModulesManager::shared()->getOperation( operation_name );    
        
        if( cmdLine.get_num_arguments() < (int)(1 + operation->getNumInputs() + operation->getNumOutputs() ) )
        {
            error.set( au::Format::string( "Not enougth parameters for operation %s" , operation_name.c_str() ) );
            return;
        }

        // Get the name of the input and output queues
		int pos = 1;
		for (int i = 0 ; i < operation->getNumInputs() ;i++)
			inputs.push_back( cmdLine.get_argument( pos++) );
		
		for (int i = 0 ; i < operation->getNumOutputs() ;i++)
			outputs.push_back( cmdLine.get_argument( pos++) );
		
	}
	
	ControllerTaskInfo::~ControllerTaskInfo()
	{

		for ( size_t i = 0 ; i < input_queues.size() ; i++)
			delete input_queues[i];		
		
		for ( size_t i = 0 ; i < output_queues.size() ; i++)
			delete output_queues[i];		
	}
	

}
