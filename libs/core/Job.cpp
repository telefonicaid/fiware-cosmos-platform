

#include "Job.h"				// Own interface
#include "SamsonController.h"	// ss::SamsonController
#include "CommandLine.h"		// au::CommandLine
#include "samson/Operation.h"	// ss::Operation

namespace ss {


	
	bool Job::processCommand( std::string command )
	{
		
		au::CommandLine commandLine;
		commandLine.parse(command);
		
		// Direct controller commands
		// ------------------------------------------
		
		if( commandLine.get_num_arguments() > 0)
		{
			// Main command
			std::string c = commandLine.get_argument(0);
			
			// Direct known commands
			if( c == "add_queue" || c == "remove_queue" )
			{
				DataManagerCommandResponse response = controller->data.runOperation( id,  command  );
				if( response.error )
				{
					setError( response.output );
					return false;
				}
				return true;
			}
				

			Operation *operation = controller->modulesManager.getOperation( c );
			
			if(  operation && (operation->getType() == Operation::generator ) ) 
			{
				// No validation of the arguments rigth now... ( just for testing )
				task_id = controller->taskManager.addTask( command , id );
				return false;	// No continue until confirmation of this task is received
			}
			
			if(  operation && (operation->getType() == Operation::script ) ) 
			{
				// Create a JobItem for this script, push into the task and return true to continue
				JobItem i( command );
				
				// Rigth now we do not "modify" internal script code
				for (size_t c = 0 ; c < operation->code.size() ; c++)
				{
					std::string command = operation->code[c];
					i.addCommand( command );
				}
				
				items.push_back(i);
				return true;
			}
			
			// Unknown command, so inmediate answer with error
			setError( "Unknown command");
			return false;
			
		}
		
		setError( std::string("Command without any command: ") + command);
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
		controller->sendDelilahAnswer( sender_id, fromIdentifier, error, true, mainCommand, output.str());
	}
	
	void Job::setError( std::string txt )
	{
		output << "Error at..." << std::endl;
		std::list<JobItem>::iterator i;
		for (i = items.begin() ; i != items.end() ; i++)
			output << ">> " << i->parent_command << std::endl;
		output << "\n>>>> Error: " << txt << std::endl;
		
		error = true;
		finish = true;	
	}
	
	
	
	
	
}

