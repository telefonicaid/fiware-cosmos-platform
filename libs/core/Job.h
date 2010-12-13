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
	
		void fill( network::JobItem *item)
		{
			item->set_command( parent_command );
			item->set_num_lines( command.size() );
			item->set_line( command_pos - 1);
		}
		
		std::string getMainCommand()
		{
			return parent_command;
		}
		
		
		double getProgress()
		{
			return (double) (command_pos-1) / (double) command.size();
		}
		
	};
	
	class JobManager;
	
	/**
	 A job ( ordered from delilah or from another job)
	 */
	
	class Job
	{
		size_t id;						// Identifier of the job ( this is the one reported to delialh to monitorize a job)
		
		std::string error_line;			// One line message for the error ( used in the cancel message of the data log)
		std::string error_message;		// Complete erro description
		bool error;						// Error flag
		bool finish;					// Flag to indicate that this job is finished
		
		int fromIdentifier;				// Identifier of the delailah that ordered this job
		int sender_id;					// Identifier at the sender side (the same delilah could send multiple jobs)
					
		JobManager *jobManager;			// Pointer to the job manager
		
		// Information about current running task
		size_t task_id;					// Id of the task we are waiting ( to avoid confusions )
		std::string task_command;		// Command we are currently running
		int task_num_items;				// Number of items for this task
		int task_num_finish_items;		// Finish number of items for this task
		
		std::list<JobItem> items;		// Stack of items that we are running
		
		// List of all tasks ( all of them have to be completed before considering the job completed)
		std::set<size_t> all_tasks;
		
		std::string mainCommand;		// Main command that originated this job
		
		friend class ControllerTaskManager;
		friend class ControllerTask;
		friend class JobManager;
		
		Environment environment;		// Environment properties for this job ( can be updated in runtime )
		
		
		
	public:
		
		// Constructor used for top-level jobs form delilah direct message
		
		Job( JobManager * _jobManager , size_t _id, int fromId, const network::Command &command , size_t _sender_id  );

		/**
		 Run a line of command for this job.
		 It can be an inmediate command interacting with dataManager like add_queue / remove_queue / etc... ( return true)
		 It can be a script, so a new JobItem is created and push ( return true )
		 It can be a command that is send to the workers creating a task ( return false )
		 */
		
		bool processCommand( std::string command );		
		
		/**
		 Main routine to run commands until waiting for task confirmation or the job finishs
		 */
		
		void run();

		// Call back received when a task is finished and completed
		void notifyCurrentTaskFinish( bool _error, std::string _error_message );		
		
		
		size_t getId();	

		bool isFinish();
		bool isError();
	
		void sentConfirmationToDelilah( );
		
		
		bool allTasksFinished();
		void removeTasks();

		
		std::string getErrorLine()
		{
			return error_line;
		}
		
		void fill( network::Job *job);
		
		
		
		
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
