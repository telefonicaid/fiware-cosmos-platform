
#ifndef _H_CONTROLLER_TASK_INFO
#define _H_CONTROLLER_TASK_INFO

#include <string>
#include <vector>
#include "CommandLine.h"				// au::CommandLine

namespace ss {

	class Operation;

	class ControllerTaskInfo
	{
		
	public:
		
		ControllerTaskInfo( Operation *operation  , au::CommandLine *cmdLine );
		
		void setError( std::string _error_message);
		
		std::string command;
		
		std::string operation_name;		// Name of the operation
		Operation * operation;			// Pointer to the operation
		
		std::vector<std::string> inputs;		// Name of the input queues
		std::vector<std::string> outputs;		// Name of the output queues
		
		// Information about the input files 
		// TODO: pending
		
		bool error;
		std::string error_message;
	};

}

#endif