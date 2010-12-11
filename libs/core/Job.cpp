

#include "Job.h"				// Own interface
#include "SamsonController.h"	// ss::SamsonController
#include "CommandLine.h"		// au::CommandLine
#include "samson/Operation.h"	// ss::Operation
#include "Packet.h"				// ss::Packet
#include "ControllerTask.h"		// ss::ControllerTaskInfo

namespace ss {

	
#pragma mark Job

	void Job::run()
	{
		
		while( !finish && !error && items.size() > 0)	// While there is something to process
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
				{
					return;	// No continue since a task has been scheduled ( or an error ocurred )
				}
			}
		}
		
		if ( error )
			finish = true;	// Just to make sure we cancel everything
		
		// Mark the end of the job if there are no more elements to process
		if( items.size() == 0)
		{
			finish = true;
		}
		
		
	}	
	bool Job::processCommand( std::string command )
	{
		
		// Log into data a comment to show this
		controller->data.addComment(id, std::string("PROCESS: ") + command );
		
		au::CommandLine commandLine;
		commandLine.set_flag_boolean("c");	/// Flag to create outputs as needed
		commandLine.set_flag_boolean("nc");	/// Flag to anulate the effect of -c
		commandLine.parse(command);
		
		// Direct controller commands
		// ------------------------------------------
		
		if( commandLine.get_num_arguments() > 0)
		{
			// Main command
			std::string c = commandLine.get_argument(0);
			
			// set command
			if( c == "set" )
			{
				if ( commandLine.get_num_arguments() < 3 )
				{
					setError("JobManager","Not enougth parameters for set command. Usaeg set variable value");
					return false;
				}
				
				// Set the environment variable
				environment.set( commandLine.get_argument(1) , commandLine.get_argument(2) );
				return true;
				
			}
			
			// Get the operation to run
			Operation *operation = controller->modulesManager.getOperation( c );
			
			if( operation )
			{
				
				if( commandLine.get_num_arguments() < (int)(1 + operation->getNumInputs() + operation->getNumOutputs() ) )
				{
					setError("JobManager","Not enougth parameters");
					return false;
				}

				
				// Spetial case if -c flag is activated and -nc is not pressent (it is like an script)
				if( commandLine.get_flag_bool("c") && (!commandLine.get_flag_bool("nc")) )
				{
					// Add comment to data manager to log that a script is initiated
					controller->data.addComment(id, std::string("Expanding -c optiona of: ") + command );
					
					// Create a JobItem for this script, push into the task and return true to continue
					JobItem jobItem( command );
					
					// Create all the output queues if necessary ( -f flag used to avoid errors )

					for (int i = 0 ; i < operation->getNumOutputs() ; i++)
					{
						KVFormat format = operation->getOutputFormat(i);
						std::ostringstream local_command;
						local_command << "add_queue " << commandLine.get_argument(1 + operation->getNumInputs() + i);
						local_command << " " << format.keyFormat << " " << format.valueFormat << " -f";
						jobItem.addCommand( local_command.str() );
					}
					
					jobItem.addCommand( command + " -nc" );
					items.push_back(jobItem);
					
					return true;
				}
				
				
				ControllerTaskInfo *task_info = new ControllerTaskInfo( this, operation , &commandLine );

				controller->data.retreveInfoForTask( task_info );

				if( task_info->error )
				{
					setError("Data Manager",task_info->error_message);	// There was an error with input/output parameters
					delete task_info;
					return false;
				}
				
				if( operation->getType() == Operation::generator ) 
				{
					task_id = controller->taskManager.addTask( task_info , id );
					return false;	// No continue until confirmation of this task is received
				}

				if( operation->getType() == Operation::parser ) 
				{
					task_id = controller->taskManager.addTask( task_info , id );
					return false;	// No continue until confirmation of this task is received
				}

				if( operation->getType() == Operation::map ) 
				{
					task_id = controller->taskManager.addTask( task_info , id );
					
					return false;	// No continue until confirmation of this task is received
				}

				if( operation->getType() == Operation::parserOut ) 
				{
					task_id = controller->taskManager.addTask( task_info , id );
					
					return false;	// No continue until confirmation of this task is received
				}
				
				
				if( operation->getType() == Operation::reduce ) 
				{
					task_id = controller->taskManager.addTask( task_info , id );
					return false;	// No continue until confirmation of this task is received
				}
				
				if( operation->getType() == Operation::script ) 
				{
					
					// Add comment to data manager to log that a script is initiated
					controller->data.addComment(id, std::string("SCRIPT IN:") + command );
					
					// Create a JobItem for this script, push into the task and return true to continue
					JobItem jobItem( command );
					
					// Rigth now we do not "modify" internal script code
					for (size_t c = 0 ; c < operation->code.size() ; c++)
					{
						std::string command = operation->code[c];
						
						// Replace $1-$N for real names
						for (int i = 0  ; i < commandLine.get_num_arguments() - 1 ; i++)
						{
							std::ostringstream param;
							param << "$" << (i+1);
							find_and_replace( command , param.str() , commandLine.get_argument(i+1) );
						}
						
						jobItem.addCommand( command );
					}
					
					items.push_back(jobItem);
					return true;
				}
			
				// Unknown command, so inmediate answer with error
				setError( "JobManager",  command + " : Operation currently not supported... come back soon!");
				return false;
			}
			else
			{
				// It not any operation, run directly to the data manager to process
				DataManagerCommandResponse response = controller->data.runOperation( id,  command  );
				if( response.error )
				{
					setError( "DataManager", response.output );
					return false;
				}
				return true;
				
			}
			
		}
		
		setError( "JobManager", std::string("Command without any command: ") + command);
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
		
		Packet *p2 = new Packet();
		
		network::CommandResponse *response = p2->message.mutable_command_response();
		response->set_command(mainCommand);

		if( error )
		{
			response->set_error_message( output.str() );
			response->set_error_job_id( id );
		}
		else 
			response->set_finish_job_id( id );

		// global sender id of delilah
		p2->message.set_delilah_id( sender_id );
		
		controller->network->send(controller, fromIdentifier, Message::CommandResponse, p2);
	}
	
	void Job::setError( std::string agent ,  std::string txt )
	{
		// error line
		error_line = txt;
		
		output << "Error detected by " << agent << " at..." << std::endl;
		std::list<JobItem>::iterator i;
		for (i = (++items.begin()) ; i != items.end() ; i++)
			output << ">> " << i->parent_command << std::endl;
		
		// Current line in the current item
		JobItem& item = items.back();
		output << ">> " << item.getLastCommand() << std::endl;

		
		output << "\n>>>> Error: " << txt << std::endl;
		
		error = true;
		finish = true;	
	}
	
}

