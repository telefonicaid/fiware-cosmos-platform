

#include "Job.h"				// Own interface
#include "samson/controller/SamsonController.h"	// samson::SamsonController
#include "au/CommandLine.h"		// au::CommandLine
#include "samson/module/Operation.h"	// samson::Operation
#include "samson/network/Packet.h"				// samson::Packet
#include "ControllerTask.h"		// samson::ControllerTaskInfo

namespace samson {

	
#pragma mark Job

	Job::Job( JobManager * _jobManager , size_t _id, int fromId, const network::Command &_command , size_t _sender_id  )
	{
        
        // copy the original message
        command = new network::Command(); 
        command->CopyFrom( _command );

		// By default no current task
		currenTask = NULL;
		
		// Init time for this job
		time_init = time(NULL);
		
		// Keep a pointer to the controller
		jobManager = _jobManager;
		
		// Get the main command
		mainCommand = command->command();
		
		// Keep the id of the job
		id = _id;
		
		fromIdentifier = fromId;
		sender_id = _sender_id;
		
		// Get the environment variables
		copyEnviroment( command->environment() , &environment );
		
		// Create the first item of this job
		JobItem j("TOP");
		j.addCommand(command->command());
		items.push_back( j );
		
		// Default value for the internal flags
		_status = running;
	}

    Job::~Job()
	{
        // Remove the copy of the command
        delete command;
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
        
        // Put null internal pointer to current task
        if ( ( _status == finish) || ( _status == error)  || ( _status == saving )  )
            currenTask = NULL;
		
	}
	
	bool Job::_processCommand( std::string command )
	{
		

        // If the command is a select, change it!!!
        
        // ------------------------------------------------------------------------------------------------
        au::CommandLine selectCommandLine;
        selectCommandLine.set_flag_boolean("select_complete");
        selectCommandLine.set_flag_boolean("create");
        selectCommandLine.parse( command );
        if( ( selectCommandLine.get_argument(0) == "select" ) && !selectCommandLine.get_flag_bool("select_complete") )
        {
            au::ErrorManager select_error;
            jobManager->controller->data.completeSelect( command , select_error);
            
            if( select_error.isActivated() )
            {
                setError("Select",  select_error.getMessage() );
                return false;
            }
            LM_M(("Complete select command: %s", command.c_str()));
            
            if( selectCommandLine.get_flag_bool("create") )
            {
                command.append(" -ncreate ");

                au::CommandLine cmdLine;
                cmdLine.set_flag_string("output_key_format", "no-format");
                cmdLine.set_flag_string("output_value_format", "no-format");
                cmdLine.parse(command);
                
                std::string add_command = au::str("add %s %s %s", 
                            selectCommandLine.get_argument(2).c_str() ,  
                            cmdLine.get_flag_string("output_key_format").c_str() , 
                            cmdLine.get_flag_string("output_value_format").c_str()
                                    );

                LM_M(("Command to add output queue: %s" , add_command.c_str()));
                DataManagerCommandResponse response = jobManager->controller->data.runOperation( id,  add_command  );

                
            }
            
            
        }
        // ------------------------------------------------------------------------------------------------

        
        
		// Log into data a comment to show this
		jobManager->controller->data.addComment(id, std::string("PROCESS: ") + command );

		// Simple commandLine to get the "main command"
		au::CommandLine commandLine;
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
				
			
				ControllerTaskInfo *task_info = new ControllerTaskInfo( this, command  );
                
                if( task_info->error.isActivated() )
                {
					setError("Data Manager", task_info->error.getMessage() );	// There was an error with input/output parameters
					delete task_info;
					return false;
                }
                
				jobManager->controller->data.retreveInfoForTask( id , task_info );
																
				if( task_info->error.isActivated() )
				{
					setError("Data Manager",task_info->error.getMessage() );	// There was an error with input/output parameters
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
		
        // Send a packet to delilah
        
        Packet *p2 = new Packet( Message::CommandResponse );
        
        network::CommandResponse *response = p2->message->mutable_command_response();
        response->mutable_command()->CopyFrom(*command);
        
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
        
        jobManager->controller->network->send( fromIdentifier, p2);
	}
	
	void Job::setError( std::string agent ,  std::string txt )
	{
        
        if ( _status != error ) // Keep only the first error message for this job
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
        }
        
		setStatus( error );
	}
	
    void Job::getInfo( std::ostringstream& output)
	{
        au::xml_open(output , "job" );
        
        au::xml_simple( output , "id" , id );
        
		switch (_status) {
			case error:
                au::xml_simple( output , "status" , "error" );
				break;
			case saving:
                au::xml_simple( output , "status" , "writing" );
				break;
			case running:
                au::xml_simple( output , "status" , "running" );
                au::xml_simple( output , "time" , au::time_string( difftime( time(NULL), time_init ) ) );
				break;
			case finish:
                au::xml_simple( output , "status" , "finish" );
		}
        
        if( currenTask )
            au::xml_single_element(output, "current_task", currenTask );
     
        au::xml_simple(output, "command",  mainCommand );
        
        au::xml_iterate_list_object(output, "job_items", items);
        
        au::xml_close(output , "job" );
		
	}
	

	void Job::kill( std::string message )
	{
		// Set as an error 
        setError("System", message );
		
		// Send a kill worker task to each worker for each task ( only one is active )
		for ( std::set<size_t>::iterator t =  all_tasks.begin() ; t != all_tasks.end() ; t++)
			for (int i = 0 ; i < jobManager->controller->num_workers ; i++ )
			{
				Packet *p = new Packet( Message::WorkerTaskKill );
				p->message->mutable_worker_task_kill()->set_task_id( *t );

				jobManager->controller->network->sendToWorker(i, p);
				
			}

		
	}
	
	
	
}

