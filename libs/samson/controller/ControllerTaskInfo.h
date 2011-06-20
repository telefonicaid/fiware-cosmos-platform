
#ifndef _H_CONTROLLER_TASK_INFO
#define _H_CONTROLLER_TASK_INFO

#include <string>
#include <vector>

#include "au/CommandLine.h"			// au::CommandLine
#include "au/Error.h"               // au::Error

#include "samson/common/samson.pb.h"
#include "samson/module/Environment.h"				// samson::Environment


namespace samson {

	class Operation;
	class Job;

	class ControllerTaskInfo
	{
		
	public:
		
		ControllerTaskInfo( Job *job , std::string command );
		~ControllerTaskInfo();

        Environment environment;
        
		Job *job;	// Pointer to a job ( used to retrieve environment variables )
		
        Operation*operation;                            // Pointer to the operation
        
		std::string operation_name;						// Name of the operation
		
		std::vector<std::string> inputs;				// Name of the input queues
		std::vector<std::string> outputs;				// Name of the output queues
		
        bool flag_create;    // Spetial flag to create output directories
        bool flag_clear;     // Sptial flag to clear output directories
        
		// Information about input and output queues ( inseted by DataManager )
		std::vector<network::FullQueue*> input_queues;		
		std::vector<network::FullQueue*> output_queues;		

		// Error management
        au::Error error;
		
	};
	
}

#endif