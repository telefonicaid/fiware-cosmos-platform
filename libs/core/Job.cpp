

#include "Job.h"				// Own interface
#include "SamsonController.h"	// ss::SamsonController
#include "CommandLine.h"		// au::CommandLine

namespace ss {


	
	bool Job::processCommand( std::string command )
	{
		
		au::CommandLine commandLine;
		commandLine.parse(command);
		
		// Direct controller commands
		// ------------------------------------------
		
		if( commandLine.get_num_arguments() > 0)
		{
			if( commandLine.get_argument(0) == "add_queue" )
			{
				// Add queue command
				if( commandLine.get_num_arguments() < 4 )
				{
					setError( "Usage: add_queue name <keyFormat> <valueFormat>" );
					return true;
				}
				
				std::string name = commandLine.get_argument( 1 );
				std::string keyFormat= commandLine.get_argument( 2 );
				std::string	valueFormat = commandLine.get_argument( 3 );
				
				if( !controller->modulesManager.checkData( keyFormat ) )
				{
					setError("Unsupported data type");
					output << "Unsupported data format " + keyFormat + "\n";
					return true;
				}
				
				if( !controller->modulesManager.checkData( valueFormat ) )
				{
					setError("Unsupported data type");
					output << "Unsupported data format " + keyFormat + "\n";
					return true;
				}
				
				// Check if queue exist
				if( controller->data.existQueue( name ) )
				{
					setError("Queue already exist");
					output << "Queue " + name + " already exist\n";
					return true;
				}
				
				// Add the queue
				controller->data.runOperationOfTask( id , command );
				return true;
				
			}

			// Normal taks
			// ------------------------------------------
			// controller->taskManager.addTask(XXX)
			// return false;

			// Scripts : ( create a new JobItem and push)
			// ------------------------------------------
			// return
			
			
			// Unknown command, so inmediate answer with error
			setError( "Unknown command");
			return false;
			
		}
		
		setError("Command without any command");
		return false;
	}
	
	bool Job::isFinish()
	{
		return finish;
	}
	
	bool Job::isError()
	{
		return error;
	}
	
	size_t Job::getId()
	{
		return id;
	}
	
	void Job::sentConfirmationToDelilah( )
	{
		assert( finish );
		controller->sendDelilahAnswer( sender_id, fromIdentifier, error, true, output.str());
	}
	
	
	
}

