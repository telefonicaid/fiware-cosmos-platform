#ifndef _H_JOB
#define _H_JOB

#include "Lock.h"							// au::Lock
#include <map>								// std::map
#include <vector>							// std::vector
#include <sstream>							// ss::ostringstream
#include <list>								// std::list
#include "samson.pb.h"						// ss::network::...
#include <iostream>							// std::cout

namespace ss {
	
	class SamsonController;
	class ControllerTask;
	
	
	
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
		
	};
	
	
	
	class JobManager;
	
	/**
	 A job ( ordered from delilah or from another job)
	 */
	
	class Job
	{
		size_t id;						// Identifier of the job ( this is the one reported to delialh to monitorize a job)
		
		std::ostringstream output;		// Outputs of this job message
		bool error;						// Error flag
		bool finish;					// Flag to indicate that this job is finished
		
		int fromIdentifier;		// Identifier of the delailah that ordered this job
		int sender_id;			// Identifier at the sender side
					
		SamsonController *controller;	// Pointer to the controller
		
		size_t task_id;					// Id of the task we are waiting ( to avoid confusions )
		
		std::list<JobItem> items;		// Stack of items that we are running
		
		std::string mainCommand;
		
	public:
		
		// Constructor used for top-level jobs form delilah direct message
		
		Job( SamsonController *_controller , size_t _id, size_t _fromIdentifier , int _sender_id , std::string c  )
		{
			mainCommand = c;
			
			controller = _controller;
			
			id = _id;
			
			fromIdentifier = _fromIdentifier;
			sender_id = _sender_id;

			// Create the first item of this job
			
			JobItem j("TOP");
			j.addCommand(c);
			
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
		
		void run()
		{
			
			while( !finish && !error && items.size() > 0)	// While there si something to process
			{
				
				JobItem& item = items.back();
				
				if( item.isFinish() )
				{
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
		

		// Call back received when a task is finished
		void notifyTaskFinish( size_t _task_id , std::vector<network::WorkerTaskConfirmation> &confirmationMessages )
		{
			// Lock confirmation messages to see if there are errors
			for (size_t i = 0 ; i < confirmationMessages.size(); i++)
			{
				if( confirmationMessages[i].error() )
				{
					error = true;
					output << "Error: " << confirmationMessages[i].error_message() << std::endl;
				} 
			}
			
			
			assert( task_id == _task_id );
			run();
		}
		
		
		size_t getId();	

		bool isFinish();

		bool isError();
	
		void sentConfirmationToDelilah( );
		
	private:
		
		void setError( std::string txt );
		
		
	};
	
	
	
}

#endif
