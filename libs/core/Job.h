#ifndef _H_JOB
#define _H_JOB

#include "Lock.h"							// au::Lock
#include <map>								// std::map
#include <vector>							// std::vector
#include <sstream>							// ss::ostringstream

namespace ss {
	
	class SamsonController;
	class ControllerTask;
	
	class JobManager;
	
	/**
	 A job ( ordered from delilah or from another job)
	 */
	
	class Job
	{
		size_t id;				// Identifier of the job ( this is the one reported to delialh to monitorize a job)
		
		int fromIdentifier;		// Identifier of the delailah that ordered this job
		int sender_id;			// Identifier at the sender side
				
		// List of commands
		std::vector<std::string> command;
		int command_pos;
		
		bool finish;					// Finish flag to mark the end of the process
		bool error;						// Error flag when something went wrong
					
		std::ostringstream output;		// Outputs of this job message
		
		SamsonController *controller;	// Pointer to the controller
		
		size_t task_id;		// Id of the task we are waiting ( to avoid confusions )
		
	public:
		
		// Constructor used for top-level jobs form delilah direct message
		
		Job( SamsonController *_controller , size_t _id , size_t _fromIdentifier , int _sender_id , std::string c  )
		{
			//std::cout << "Job ID " << _id << "FROM " << _fromIdentifier << " SENDER: " << _sender_id;
			
			controller = _controller;
			id = _id;
			fromIdentifier = _fromIdentifier;
			sender_id = _sender_id;
			
			command_pos = 0;			// Start by default with the first command
			finish = false;				// Finish flag when all commands are completed
			controller = _controller;	// Need the controller pointer for a lot of reasons
			
			command.push_back( c );
		}
		
		void run( );
		
		void notifyTaskFinish( size_t _task_id );		
		
		bool processCommand( std::string command );		
		
		bool isFinish();		
		
		size_t getId();	};
	
}

#endif
