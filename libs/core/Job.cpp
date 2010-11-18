

#include "Job.h"				// Own interface
#include "SamsonController.h"	// ss::SamsonController
#include "CommandLine.h"		// au::CommandLine
#include "samson/Operation.h"	// ss::Operation
#include "Packet.h"				// ss::Packet
#include "ControllerTask.h"		// ss::ControllerTaskInfo

namespace ss {


	void Job::run()
	{
		
		while( !finish && !error && items.size() > 0)	// While there si something to process
		{
			
			JobItem& item = items.back();
			
			if( item.isFinish() )
			{
				// Log into data to notify that we are out of a SCRIPT
				if( items.size() > 1)	// Not report if we are leaving the main JobItem
					controller->data.addComment(id, std::string("SCRIPT OUT:") + item.getMainCommand() );
				
				items.pop_back();
			}
			else
			{
				if( !processCommand( item.getNextCommand() ) )
					return;	// No continue since a task has been scheduled ( or an error ocurred )
			}
		}
		
		if ( error )
			finish = true;	// Just to make sure we cancel everything
		
		// Mark the end of the job if there are no more elements to process
		if( items.size() == 0)
		{
			output << "Job finished correctly\n";
			finish = true;
		}
		
	}	
	bool Job::processCommand( std::string command )
	{
		
		// Log into data a comment to show this
		controller->data.addComment(id, std::string("PROCESS: ") + command );

		
		au::CommandLine commandLine;
		commandLine.parse(command);
		
		// Direct controller commands
		// ------------------------------------------
		
		if( commandLine.get_num_arguments() > 0)
		{
			// Main command
			std::string c = commandLine.get_argument(0);
			Operation *operation = controller->modulesManager.getOperation( c );
			
			if( operation )
			{
				
				if( commandLine.get_num_arguments() < (int)(1 + operation->getNumInputs() + operation->getNumOutputs() ) )
				{
					setError("Not enougth parameters");
					return false;
				}
				
				ControllerTaskInfo *task_info = new ControllerTaskInfo( operation , &commandLine );

				
				controller->data.retreveInfoForTask( task_info );

				if( task_info->error )
				{
					setError(task_info->error_message);	// There was an error with input/output parameters
					delete task_info;
					return false;
				}
				
				if( operation->getType() == Operation::generator ) 
				{
					// No validation of the arguments rigth now... ( just for testing )
					task_id = controller->taskManager.addTask( task_info , id );
					return false;	// No continue until confirmation of this task is received
				}
				
				if( operation->getType() == Operation::script ) 
				{
					
					// Add comment to data manager to log that a script is initiated
					controller->data.addComment(id, std::string("SCRIPT IN:") + command );
					
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
				setError( command + " : Operation currently not supported... come back soon!");
				return false;
			}
			else
			{
				// It not any operation, run directly to the data manager to process
				
				DataManagerCommandResponse response = controller->data.runOperation( id,  command  );
				if( response.error )
				{
					setError( response.output );
					return false;
				}
				return true;
				
			}
			
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
		
		Packet p2;
		
		network::CommandResponse *response = p2.message.mutable_command_response();
		response->set_command(mainCommand);
		response->set_response(output.str());
		response->set_error(error);
		response->set_finish(true);
		response->set_sender_id(sender_id);
		
		controller->network->send(controller, fromIdentifier, Message::CommandResponse, &p2);
	}
	
	void Job::setError( std::string txt )
	{
		// error line
		error_line = txt;
		
		output << "Error at..." << std::endl;
		std::list<JobItem>::iterator i;
		for (i = items.begin() ; i != items.end() ; i++)
			output << ">> " << i->parent_command << std::endl;
		
		// Current line in the current item
		JobItem& item = items.back();
		output << ">> " << item.getLastCommand() << std::endl;

		
		output << "\n>>>> Error: " << txt << std::endl;
		
		error = true;
		finish = true;	
	}
	
	
	
	
	
}

