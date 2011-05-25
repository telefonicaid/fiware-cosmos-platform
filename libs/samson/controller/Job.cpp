

#include "Job.h"				// Own interface
#include "samson/controller/SamsonController.h"	// samson::SamsonController
#include "au/CommandLine.h"		// au::CommandLine
#include "samson/module/Operation.h"	// samson::Operation
#include "samson/network/Packet.h"				// samson::Packet
#include "ControllerTask.h"		// samson::ControllerTaskInfo

namespace samson {

	
#pragma mark Job

	Job::Job( JobManager * _jobManager , size_t _id, int fromId, const network::Command &command , size_t _sender_id  )
	{
		// By default no current task
		currenTask = NULL;
		
		// Init time for this job
		time_init = time(NULL);
		
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
		_status = running;
	}
	
	
	void Job::run()
	{
		while( (_status == running) && items.size() > 0)	// While there is something to process
		{
			
			JobItem& item = items.back();
			
			if( item.isFinish() )
			{
				// Log into data to notify that we are out of a SCRIPT
				if( items.size() > 1)	// Not report if we are leaving the main JobItem
					jobManager->controller->data.addComment(id, std::string("SCRIPT OUT:") + item.getMainCommand() );

				item.clear();
				
				items.pop_back();
			}
			else
			{
				
				if( !_processCommand( item.getNextCommand() ) )
					return;	// No continue since a task has been scheduled ( or an error ocurred )
			}
		}
				
		// Mark the end of the job if there are no more elements to process
		if( items.size() == 0)
		{
			if( allTasksCompleted() )
				setStatus( finish );
			else
				setStatus( saving );
		}
		
	}	
	
	
	void Job::setStatus( JobStatus s )
	{
		// Once in error or finish, we cannot set anything else
		if ( _status == finish )
		{
			if ( s != finish)
				LM_W(("Seting a different state for a finished job"));
			return;
		}

		if ( _status == error )
		{
			if ( s != error)
				LM_W(("Seting a different state for a error-finished job"));
			return;
		}
				
		// Set the status
		_status = s;
		
		if( _status == error )
		{
			sentConfirmationToDelilah();
			jobManager->controller->data.cancelTask( getId() , "Job error" );
		}

		if( _status == finish )
		{
			sentConfirmationToDelilah();
			jobManager->controller->data.finishTask( getId() );
		}

		
	}
	
	bool Job::_processCommand( std::string command )
	{
		
		// Log into data a comment to show this
		jobManager->controller->data.addComment(id, std::string("PROCESS: ") + command );
		
		au::CommandLine commandLine;
		commandLine.set_flag_boolean("create");	/// Flag to create outputs as needed
		commandLine.set_flag_boolean("ncreate");	/// Flag to anulate the effect of -c
		commandLine.set_flag_boolean("clear");	/// Flag to clear outputs before emiting key-values
		commandLine.set_flag_boolean("nclear");	/// Flag to anulate the effect of -clear
		commandLine.set_flag_boolean("clear_inputs");	/// Flag to clear inputs after emiting key-values
		
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
			Operation *operation = ModulesManager::shared()->getOperation( c );
			
			if( operation )
			{
				
				if( commandLine.get_num_arguments() < (int)(1 + operation->getNumInputs() + operation->getNumOutputs() ) )
				{
					setError("JobManager","Not enougth parameters");
					return false;
				}

				
				// Spetial case if -c flag is activated and -nc is not pressent (it is like a script)
				if( commandLine.get_flag_bool("create") && (!commandLine.get_flag_bool("ncreate")) )
				{
					// Add comment to data manager to log that a script is initiated
					jobManager->controller->data.addComment(id, std::string("Expanding -c option of: ") + command );
					
					// Create a JobItem for this script, push into the task and return true to continue
					JobItem jobItem( command );
					
					// Create all the output queues if necessary ( -f flag used to avoid errors )

					for (int i = 0 ; i < operation->getNumOutputs() ; i++)
					{
						KVFormat format = operation->getOutputFormat(i);
						std::ostringstream local_command;
						local_command << "add " << commandLine.get_argument(1 + operation->getNumInputs() + i);
						local_command << " " << format.keyFormat << " " << format.valueFormat << " -f";
						jobItem.addCommand( local_command.str() );
					}
					
					jobItem.addCommand( command + " -ncreate" );
					items.push_back(jobItem);
					
					return true;
				}

				// Spetial case if -clear flag is activated and -nclear is not pressent (it is like a script)
				if( commandLine.get_flag_bool("clear") && (!commandLine.get_flag_bool("nclear")) )
				{
					// Add comment to data manager to log that a script is initiated
					jobManager->controller->data.addComment(id, std::string("Expanding -clear option of: ") + command );
					
					// Create a JobItem for this script, push into the task and return true to continue
					JobItem jobItem( command );
					
					// Create all the output queues if necessary ( -f flag used to avoid errors )
					
					for (int i = 0 ; i < operation->getNumOutputs() ; i++)
					{
						{
							std::ostringstream local_command;
							local_command << "add  " << commandLine.get_argument(1 + operation->getNumInputs() + i);
							KVFormat format = operation->getOutputFormat(i);
							local_command << " " << format.keyFormat << " " << format.valueFormat << " -f";
							jobItem.addCommand( local_command.str() );
						}
						{
							std::ostringstream local_command;
							local_command << "clear " << commandLine.get_argument(1 + operation->getNumInputs() + i);
							jobItem.addCommand( local_command.str() );
						}
					}
					
					jobItem.addCommand( command + " -nclear" );
					items.push_back(jobItem);
					
					return true;
				}
				
				ControllerTaskInfo *task_info = new ControllerTaskInfo( this, operation , &commandLine );
				
				jobManager->controller->data.retreveInfoForTask( id , task_info , commandLine.get_flag_bool("clear_inputs" ) );
																
																
				if( task_info->error )
				{
					setError("Data Manager",task_info->error_message);	// There was an error with input/output parameters
					delete task_info;
					return false;
				}
				
				
				if( operation->getType() != Operation::script ) 
				{
					// Reset task information
					
					currenTask = jobManager->taskManager.addTask( task_info , this );
					
					// Insert this task in the list of pending to be confirmed list of task id
					all_tasks.insert( currenTask->getId() );
					
					return false;	// No continue until confirmation of this task is received
				}
                else
				{
                    
                    // Task info is not used any more in the case of a script
                    delete task_info;
					
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
		
		setError( "JobManager", std::string("Command without any command: ") + command );
		return false;
	}
	
	bool Job::isCurrentTask( ControllerTask *task)
	{
		return (currenTask == task);
	}
	
	bool Job::allTasksCompleted()
	{
		std::set<size_t>::iterator iter;
		for(iter = all_tasks.begin() ; iter != all_tasks.end() ; iter++)
		{
			ControllerTask *task = jobManager->taskManager.getTask( *iter );

			if( task->getState() != ControllerTask::completed )
				return false;
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
	
	void Job::notifyCurrentTaskFinish(  )
	{
        run();	// Continue execution
	}
    
    void Job::notifyCurrentTaskReportedError( std::string _error_message )
    {
        setError( "task at workers" ,  _error_message );
        run();
    }
	
	void Job::sentConfirmationToDelilah( )
	{
		// Only error of finished jobs can send this message
		if( ( _status != finish) && ( _status != error ) )
			LM_X(1,("Unexpected status when sending confirmation to delilah"));
		
		if( fromIdentifier == -1)
		{
			jobManager->controller->data.finishAutomaticOperation( sender_id , error , error_message );
			//std::cout << "Finish internal job with id " << id << "\n";
		}
		else
		{
			// Send a packet to delilah
			
			Packet *p2 = new Packet( Message::CommandResponse );
			
			network::CommandResponse *response = p2->message->mutable_command_response();
			response->set_command(mainCommand);

			if( _status == error )
			{
				response->set_error_message( error_message );
				response->set_error_job_id( id );
			}
			else 
			{
				response->set_finish_job_id( id );
			}

			// Inform about ellapsed time
			time_t time_finish = time(NULL);
			response->set_ellapsed_seconds( difftime( time_finish, time_init ) );
										  
			// global sender id of delilah
			p2->message->set_delilah_id( sender_id );
			
			jobManager->controller->network->send(jobManager->controller, fromIdentifier, p2);
		}
	}
	
	void Job::setError( std::string agent ,  std::string txt )
	{
		error_line = txt;			// error line
		
		// String with the error
		std::ostringstream output;

		output << "Error detected by " << agent << " at..." << std::endl;
		std::list<JobItem>::iterator i;
		for (i = items.begin() ; i != items.end() ; i++)
			output << ">> " << i->parent_command << std::endl;
		
		// Current line in the current item
		JobItem& item = items.back();
		output << ">> " << item.getLastCommand() << std::endl;		
		output << "\n>>>> Error: " << txt << std::endl;

		// Set the error status 
		error_message = output.str();
		setStatus( error );
	}
	
	void Job::fill( network::Job *job )
	{
		job->set_id( id );
		job->set_status( getStatus() );

		if( status() == error )
			job->set_main_command( mainCommand + " [ " + error_line + " ]" );
		else 
		{
			job->set_main_command( mainCommand );
		
			if ( _status == running )
			{
				std::list<JobItem>::iterator iter;
				for ( iter=items.begin() ; iter != items.end() ; iter++ )
				{
					network::JobItem *j = job->add_item();
					iter->fill( j );
				}
				
				if (( currenTask ))
				{
					// Artifitial jobitem to show current task
					network::JobItem *j = job->add_item();
					j->set_command( currenTask->info->operation_name );
					j->set_line( 0 );
					j->set_num_lines( 0 );
				}
			}
		}
	}

	std::string Job::getStatus()
	{
		std::ostringstream output;
        output << "id " << id << " ";
        
		switch (_status) {
			case error:
				output << "Error";
				break;
			case saving:
				output << "Writing";
				break;
			case running:
				output << "Running " << au::Format::time_string( difftime( time(NULL), time_init ) );
				break;
			case finish:
				output << "Finished";
		}
		
		return output.str();
	}
	

	void Job::kill()
	{
		// Set the error status 
		error_message = "Killed by user";
		setStatus( error );
		
		// Send a kill worker task to each worker for each task (only one is active )
		for ( std::set<size_t>::iterator t =  all_tasks.begin() ; t != all_tasks.end() ; t++)
			for (int i = 0 ; i < jobManager->controller->num_workers ; i++ )
			{
				Packet *p = new Packet( Message::WorkerTaskKill );
				p->message->mutable_worker_task_kill()->set_task_id( *t );
				NetworkInterface *network = jobManager->controller->network;
				network->send(jobManager->controller, network->workerGetIdentifier(i), p);
				
			}

		
	}
	
	
	
}

