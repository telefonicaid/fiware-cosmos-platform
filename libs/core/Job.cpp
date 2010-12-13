

#include "Job.h"				// Own interface
#include "SamsonController.h"	// ss::SamsonController
#include "CommandLine.h"		// au::CommandLine
#include "samson/Operation.h"	// ss::Operation
#include "Packet.h"				// ss::Packet
#include "ControllerTask.h"		// ss::ControllerTaskInfo

namespace ss {

	
#pragma mark Job

	Job::Job( JobManager * _jobManager , size_t _id, int fromId, const network::Command &command , size_t _sender_id  )
	{
		// Keep a pointer to the controller
		jobManager = _jobManager;
		
		// Get the main command
		mainCommand = command.command();
		
		// Keep the id of the job
		id = _id;
		
		fromIdentifier = fromId;
		sender_id = _sender_id;
		
		// Get the environment variables
		copyEnviroment( command.environment() , &environment );
		
		// Create the first item of this job
		JobItem j("TOP");
		j.addCommand(command.command());
		
		items.push_back( j );
		
		// Default value for the internal flags
		error = false;
		finish = false;
	}
	
	
	void Job::run()
	{
		while( !finish && !error && items.size() > 0)	// While there is something to process
		{
			
			JobItem& item = items.back();
			
			if( item.isFinish() )
			{
				// Log into data to notify that we are out of a SCRIPT
				if( items.size() > 1)	// Not report if we are leaving the main JobItem
					jobManager->controller->data.addComment(id, std::string("SCRIPT OUT:") + item.getMainCommand() );
				
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
			finish = true;
		
	}	
	bool Job::processCommand( std::string command )
	{
		
		// Log into data a comment to show this
		jobManager->controller->data.addComment(id, std::string("PROCESS: ") + command );
		
		au::CommandLine commandLine;
		commandLine.set_flag_boolean("c");	/// Flag to create outputs as needed
		commandLine.set_flag_boolean("nc");	/// Flag to anulate the effect of -c
		commandLine.set_flag_boolean("clear");	/// Flag to clear outputs before emiting key-values
		commandLine.set_flag_boolean("nclear");	/// Flag to anulate the effect of -clear
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

			// unset command
			if( c == "unset" )
			{
				if ( commandLine.get_num_arguments() < 2 )
				{
					setError("JobManager","Not enougth parameters for unset command. Usage unset variable");
					return false;
				}
				
				// Set the environment variable
				environment.unset( commandLine.get_argument(1) );
				return true;
				
			}
			
			
			// Get the operation to run
			Operation *operation = jobManager->controller->modulesManager.getOperation( c );
			
			if( operation )
			{
				
				if( commandLine.get_num_arguments() < (int)(1 + operation->getNumInputs() + operation->getNumOutputs() ) )
				{
					setError("JobManager","Not enougth parameters");
					return false;
				}

				
				// Spetial case if -c flag is activated and -nc is not pressent (it is like a script)
				if( commandLine.get_flag_bool("c") && (!commandLine.get_flag_bool("nc")) )
				{
					// Add comment to data manager to log that a script is initiated
					jobManager->controller->data.addComment(id, std::string("Expanding -c optiona of: ") + command );
					
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

				// Spetial case if -clear flag is activated and -nclear is not pressent (it is like a script)
				if( commandLine.get_flag_bool("clear") && (!commandLine.get_flag_bool("nclear")) )
				{
					// Add comment to data manager to log that a script is initiated
					jobManager->controller->data.addComment(id, std::string("Expanding -clear optiona of: ") + command );
					
					// Create a JobItem for this script, push into the task and return true to continue
					JobItem jobItem( command );
					
					// Create all the output queues if necessary ( -f flag used to avoid errors )
					
					for (int i = 0 ; i < operation->getNumOutputs() ; i++)
					{
						{
							std::ostringstream local_command;
							local_command << "add_queue " << commandLine.get_argument(1 + operation->getNumInputs() + i);
							KVFormat format = operation->getOutputFormat(i);
							local_command << " " << format.keyFormat << " " << format.valueFormat << " -f";
							jobItem.addCommand( local_command.str() );
						}
						{
							std::ostringstream local_command;
							local_command << "clear_queue " << commandLine.get_argument(1 + operation->getNumInputs() + i);
							jobItem.addCommand( local_command.str() );
						}
					}
					
					jobItem.addCommand( command + " -nclear" );
					items.push_back(jobItem);
					
					return true;
				}
				
				
				ControllerTaskInfo *task_info = new ControllerTaskInfo( this, operation , &commandLine );

				jobManager->controller->data.retreveInfoForTask( task_info );

				if( task_info->error )
				{
					setError("Data Manager",task_info->error_message);	// There was an error with input/output parameters
					delete task_info;
					return false;
				}
				
				
				if( operation->getType() != Operation::script ) 
				{
					// Reset task information
					task_command = command;
					task_num_items = -1;
					task_num_finish_items = -1;
					
					task_id = jobManager->taskManager.addTask( task_info , this );
					
					// Insert this task in the list of pending to be confirmed list of task id
					all_tasks.insert( task_id );
					
					return false;	// No continue until confirmation of this task is received
				}
				
				if( operation->getType() == Operation::script ) 
				{
					
					// Add comment to data manager to log that a script is initiated
					jobManager->controller->data.addComment(id, std::string("SCRIPT IN:") + command );
					
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
				DataManagerCommandResponse response = jobManager->controller->data.runOperation( id,  command  );
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
	
	bool Job::allTasksFinished()
	{
		std::set<size_t>::iterator iter;
		
		for(iter = all_tasks.begin() ; iter != all_tasks.end() ; iter++)
		{
			ControllerTask *task = jobManager->taskManager.getTask( *iter );
			if( !task->complete )
			{
				return false;
			}
		}
		
		return true;
	}

	void Job::removeTasks()
	{
		std::set<size_t>::iterator iter;
		
		for(iter = all_tasks.begin() ; iter != all_tasks.end() ; iter++)
			jobManager->taskManager.removeTask( *iter );
		
	}
	
	
	size_t Job::getId()
	{
		return id;
	}
	
	void Job::notifyCurrentTaskFinish( bool _error, std::string _error_message )
	{
		if( _error)
			setError( "task at workers" ,  _error_message );
		
		run();	// Continue execution
	}
	
	void Job::sentConfirmationToDelilah( )
	{
		assert( finish );
		
		Packet *p2 = new Packet();
		
		network::CommandResponse *response = p2->message.mutable_command_response();
		response->set_command(mainCommand);

		if( error )
		{
			response->set_error_message( error_message );
			response->set_error_job_id( id );
		}
		else 
			response->set_finish_job_id( id );

		// global sender id of delilah
		p2->message.set_delilah_id( sender_id );
		
		jobManager->controller->network->send(jobManager->controller, fromIdentifier, Message::CommandResponse, p2);
	}
	
	void Job::setError( std::string agent ,  std::string txt )
	{
		// error line
		error_line = txt;
		std::ostringstream output;
		output << "Error detected by " << agent << " at..." << std::endl;
		std::list<JobItem>::iterator i;
		for (i = items.begin() ; i != items.end() ; i++)
			output << ">> " << i->parent_command << std::endl;
		
		// Current line in the current item
		JobItem& item = items.back();
		output << ">> " << item.getLastCommand() << std::endl;

		
		output << "\n>>>> Error: " << txt << std::endl;
		
		error = true;
		error_message = output.str();
		finish = true;	
	}
	
	void Job::fill( network::Job *job)
	{
		job->set_id( id );
		job->set_main_command( mainCommand );
		
		std::list<JobItem>::iterator iter;
		for ( iter=items.begin() ; iter != items.end() ; iter++ )
		{
			network::JobItem *j = job->add_item();
			iter->fill( j );
		}
		
		if( finish )
		{
			// Artifitial jobitem to say this is finish waiting to be confirmed as completed
			network::JobItem *j = job->add_item();
			j->set_command( "Finished... waiting to be saved to disk" );
			j->set_line( 0 );
			j->set_num_lines( 0 );
		}
		else
		{
			// Artifitial jobitem to show current task
			network::JobItem *j = job->add_item();
			j->set_command( task_command );
			j->set_line( task_num_finish_items );
			j->set_num_lines( task_num_items );
		}

		
	}
	
}

