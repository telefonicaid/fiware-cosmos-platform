

#include "Job.h"				// Own interface
#include "SamsonController.h"	// ss::SamsonController
#include "CommandLine.h"		// au::CommandLine

namespace ss {

	void Job::run( )
	{
		
		if ( command_pos == (int)command.size() )
		{
			// No more commands to run
			finish = true;
			
			// Send the message back to delilah to confirm the end of this job
			controller->sendDelilahAnswer(sender_id, fromIdentifier, false, true, "Finish correctly" );
			return;
		}
		
		if( !processCommand( command[command_pos] ) )
		{
			// There is an error in this line
			error = true;
			finish = true;
			
			// Send the message back to delilah
			controller->sendDelilahAnswer(sender_id, fromIdentifier, true, true, output.str() );
		}
		else 
		{
			controller->sendDelilahAnswer(sender_id, fromIdentifier, false, false, "Processing..." );
			command_pos++;
		}
	}
	
	void Job::notifyTaskFinish( size_t _task_id )
	{
		// We only use if it is the task we were waiting from ( avoid confusions in the future )
		
		if( task_id == _task_id )
			run();
	}
	
	
	bool Job::processCommand( std::string command )
	{
		au::CommandLine commandLine;
		commandLine.parse(command);
		
		// Check everything is ok
		
		if( commandLine.get_num_arguments() > 0)
		{
			if( commandLine.get_argument(0) == "add_queue" )
			{
				// Add queue command
				if( commandLine.get_num_arguments() < 4 )
				{
					output << "Usage: add_queue name <keyFormat> <valueFormat>";
					return false;
				}
				
				std::string name = commandLine.get_argument( 1 );
				std::string keyFormat= commandLine.get_argument( 2 );
				std::string	valueFormat = commandLine.get_argument( 3 );
				
				if( !controller->modulesManager.checkData( keyFormat ) )
				{
					output << "Unsupported data format " + keyFormat + "\n";
					return false;
				}
				
				if( !controller->modulesManager.checkData( valueFormat ) )
				{
					output << "Unsupported data format " + valueFormat + "\n";
					return false;
				}
				
				// Check if queue exist
				if( controller->data.existQueue( name ) )
				{
					output << "Queue " + name + " already exist\n";
					return false;
				}
				
				// Add the task ( make sure we keep the task id)
				task_id = controller->taskManager.addTask( command , id );
				return true;
				
			}
			
			// Rigth now we do not support more jobs ;)
			return false;
		}
		
		output << "Command without any command";
		return false;
		
	}
	
	bool Job::isFinish()
	{
		return finish;
	}
	
	size_t Job::getId()
	{
		return id;
	}
	
	
}

