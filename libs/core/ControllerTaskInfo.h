
#ifndef _H_CONTROLLER_TASK_INFO
#define _H_CONTROLLER_TASK_INFO

#include <string>
#include <vector>
#include "CommandLine.h"				// au::CommandLine
#include "samson.pb.h"

namespace ss {

	class Operation;

	class ControllerTaskInfo
	{
		
	public:
		
		ControllerTaskInfo( Operation *operation  , au::CommandLine *cmdLine );

		/**
		 Function to set an error while retriving information at ControllerDataManager
		 */
		
		void setError( std::string _error_message);
		
		std::string command;
		
		std::string operation_name;				// Name of the operation
		Operation * operation;					// Pointer to the operation
		
		std::vector<std::string> inputs;				// Name of the input queues
		std::vector<std::string> outputs;				// Name of the output queues

		// Output queues with format ( inseted by DataManager )
		std::vector<network::Queue> output_queues;		
		
		// Information about the input files 
		// TODO: pending
		
		bool error;
		std::string error_message;
	};
	
}

#endif