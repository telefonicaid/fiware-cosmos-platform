#ifndef _H_JOB
#define _H_JOB

#include "Lock.h"							// au::Lock
#include <map>								// std::map
#include <vector>							// std::vector
#include <sstream>							// ss::ostringstream
#include <list>								// std::list
#include "samson.pb.h"						// ss::network::...
#include <iostream>							// std::cout
#include "Status.h"				// ss::ObjectWithStatus
#include "CommandLine.h"					// au::CommandLine
#include "samson/Environment.h"				// ss::Environment
#include "EnvironmentOperations.h"			// copyEnviroment(.)

namespace ss {
	
	/**
	 
	 Class Job
	 
	 */
	
	class SamsonController;
	class ControllerTask;
	class Operation;
	class ControllerTaskInfo;
	
	class JobItem
	{
		
	public:
		
		// Main command that originated this job item
		std::string parent_command;
		
		// List of commands
		std::vector<std::string> command;
		int command_pos;


		// Single element command
		JobItem( std::string _parent_command )	
		{
			parent_command = _parent_command;
			command_pos = 0;
		}
		
		void addCommand( std::string c)
		{
			command.push_back(c);
		}
		
		bool isFinish()
		{
			return ( command_pos == (int) command.size() );
		}
		
		std::string getNextCommand()
		{
			assert( command_pos < (int)command.size() );
			return command[command_pos++];
		}
		
		std::string getLastCommand()
		{
			if ( command_pos == 0)
				return "begining";
			if ( command_pos > (int)command.size() )
				return "error: Too far in the JobItem list of commands";
			
			return command[command_pos-1];
		}
	
		std::string getMainCommand()
		{
			return parent_command;
		}
		
	};
	
	
	
	class JobManager;
	
	/**
	 A job ( ordered from delilah or from another job)
	 */
	
	class Job
	{
		size_t id;						// Identifier of the job ( this is the one reported to delialh to monitorize a job)
		
		std::ostringstream output;		// Outputs of this job message
		std::string error_line;			// One line message for the error ( used in the cancel message of the data log)
		bool error;						// Error flag
		bool finish;					// Flag to indicate that this job is finished
		
		int fromIdentifier;				// Identifier of the delailah that ordered this job
		int sender_id;					// Identifier at the sender side (the same delilah could send multiple jobs)
					
		SamsonController *controller;	// Pointer to the controller
		
		size_t task_id;					// Id of the task we are waiting ( to avoid confusions )
		
		std::list<JobItem> items;		// Stack of items that we are running
		
		std::string mainCommand;		// Main command that originated this job
		
		friend class ControllerTaskManager;
		friend class ControllerTask;
		friend class JobManager;
		
		Environment environment;		// Environment properties for this job ( can be updated in runtime )
		
	public:
		
		// Constructor used for top-level jobs form delilah direct message
		
		Job( SamsonController *_controller , size_t _id, int fromId, const network::Command &command , size_t _sender_id  )
		{
			// Keep a pointer to the controller
			controller = _controller;
			
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

			// Need the controller pointer for a lot of reasons
			controller = _controller;	
			
			// Default value for the internal flags
			error = false;
			finish = false;
		}

		/**
		 Run a line of command for this job.
		 It can be an inmediate command interacting with dataManager like add_queue / remove_queue / etc... ( return true)
		 It can be a script, so a new JobItem is created and push ( return true )
		 It can be a command that is send to the workers creating a task ( return false )
		 */
		
		bool processCommand( std::string command );		
		
	public:
		
		/**
		 Main routine to run commands until waiting for task confirmation or another job finish
		 */
		
		void run();
		

		// Call back received when a task is finished
		void notifyTaskFinish( size_t _task_id , bool _error, std::string _error_message )
		{
			if( _error)
				setError( "task at workers" ,  _error_message );
			
			assert( task_id == _task_id );	//Make sure it is the task we were running
			run();
		}
		
		
		size_t getId();	

		bool isFinish();

		bool isError();
	
		void sentConfirmationToDelilah( );
		
		std::string getErrorLine()
		{
			return error_line;
		}
		
		std::string getStatus()
		{
			std::ostringstream output;
			output << "Job: " << mainCommand << " ";
			if( error )
				output << "[Error]";
			output << std::endl;
			
			return output.str();
		}
		
	private:
		
		void setError( std::string agent , std::string txt );
		

		// Axiliar function to replace text in strings
		static void find_and_replace( std::string &source, const std::string find, std::string replace ) {
			size_t j;
			for ( ; (j = source.find( find )) != std::string::npos ; ) {
				source.replace( j, find.length(), replace );
			}
		}	
		
		
	};
	
	
	
}

#endif
