#ifndef _H_JOB
#define _H_JOB

#include "au/Lock.h"							// au::Lock
#include <map>								// std::map
#include <vector>							// std::vector
#include <sstream>							// samson::ostringstream
#include <list>								// std::list
#include "samson/common/samson.pb.h"						// samson::network::...
#include <iostream>							// std::cout
#include "samson/module/Environment.h"				// samson::Environment
#include "samson/common/EnvironmentOperations.h"			// copyEnviroment(.)
#include <set>								// std::set
#include <samson/module/Operation.h>				// samson::Operation
#include "logMsg/logMsg.h" //LM_X

namespace samson {
	
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
		int num_lines;
		int command_pos;


		// Single element command
		JobItem( std::string _parent_command )	
		{
			parent_command = _parent_command;
			command_pos = 0;
			num_lines = 0;
		}
		
		void addCommand( std::string c)
		{
			command.push_back(c);
			num_lines++;
		}
		
		bool isFinish()
		{
			return ( command_pos == (int) num_lines );
		}
		
		std::string getNextCommand()
		{
			if( command_pos > (int) num_lines )
			  LM_X(1,("Error in Job: Current line %d, num lines %d", (int)command_pos ,(int) num_lines));
			return command[command_pos++];
		}
		
		std::string getLastCommand()
		{
			if ( command_pos == 0)
				return "begining";
			if ( command_pos > (int) num_lines )
				return "error: Too far in the JobItem list of commands";
			
			return command[command_pos-1];
		}
	
		void fill( network::JobItem *item)
		{
			item->set_command( parent_command );
			item->set_num_lines( num_lines );
			item->set_line( command_pos - 1);
		}
		
		std::string getMainCommand()
		{
			return parent_command;
		}
		
		
		double getProgress()
		{
			return (double) (command_pos-1) / (double) num_lines;
		}
		
		void clear()
		{
			command.clear();
		}
		
		
		
	};
	
	class JobManager;
	
	/**
	 A job ( ordered from delilah or from another job)
	 */
	
	class Job
	{
		
	public:
		
		typedef enum 
		{
			running,	// Running task
			saving,		// Finish but waiting to save generated files
			error,		// Finish because there was an error
			finish,		// Completelly finish
		} JobStatus;
		
	private:
		
		JobManager *jobManager;			// Pointer to the job manager
		size_t id;						// Identifier of the job ( this is the one reported to delialh to monitorize a job)

		// Basic information about this job
		time_t time_init;					// Init time of this job ( to monitor the operation )
		int fromIdentifier;					// Identifier of the delailah that ordered this job
		int sender_id;						// Identifier at the sender side (the same delilah could send multiple jobs)
		
		std::string error_line;				// One line message for the error ( used in the cancel message of the data log)
		std::string error_message;			// Complete error description
		
		JobStatus _status;	// Status of this job
		
		// Information about current running task
		ControllerTask *currenTask;
		
		std::list<JobItem> items;           // Stack of items that we are running
		
		// List of all tasks ( all of them have to be completed before considering the job completed)
		std::set<size_t> all_tasks;
		
        network::Command *command;      // Copy of the original message sent from a delilah
        
		std::string mainCommand;		// Main command that originated this job
		
	public:
		
		Environment environment;		// Environment properties for this job ( can be updated in runtime )
		
		// Constructor used for top-level jobs form delilah direct message
		
		Job( JobManager * _jobManager , size_t _id, int fromId, const network::Command &command , size_t _sender_id  );
		~Job();
        
		/**
		 Main routine to run commands until waiting for task confirmation or the job finishs
		 */
		
		void run();

		// Call back received when a task is finished and completed
		void notifyCurrentTaskFinish( );		
		void notifyCurrentTaskReportedError( std::string error_message );
		
		size_t getId();	
		JobStatus status()
		{
			return _status;
		};
		// Set the status ( sending a message to delilah if necessary )
		void setStatus( JobStatus s);

				
		bool allTasksCompleted();
		void removeTasks();
		
		bool isCurrentTask( ControllerTask *task );
		
		std::string getErrorLine()
		{
			return error_line;
		}
		
		void fill( network::Job *job );
		
		std::string getStatus();
		
		void kill(std::string message);	// Kill this job ( similar to error )

	private:
		void setError( std::string agent , std::string txt );
		void sentConfirmationToDelilah( );

		
		
	private:
		/**
		 Run a line of command for this job.
		 It can be an inmediate command interacting with dataManager like add_queue / remove_queue / etc... ( return true)
		 It can be a script, so a new JobItem is created and push ( return true )
		 It can be a command that is send to the workers creating a task ( return false )
		 */
		
		bool _processCommand( std::string command );		
		
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
