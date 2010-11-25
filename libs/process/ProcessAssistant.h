#ifndef _H_PROCESS_ASSISTANT
#define _H_PROCESS_ASSISTANT

/* ****************************************************************************
*
* FILE                     ProcessAssistant.h
*
* AUTHOR                   Andreu Urruela / Ken Zangelin
*
* CREATION DATE            Sep 2010
*
*/
#include <string>					// std::string
#include <pthread.h>				// pthread_t
#include "ProcessAssistant.h"		// ss::ProcessAssistant
#include <sstream>					// std::stringstream
#include <iostream>					// std::cout
#include "ProcessAssistantBase.h"	// ss::ProcessAssistantBase

namespace ss {
	

	 
/* ****************************************************************************
*
* ProcessAssistant - 
*
* Class that is in charge of managing an independent process running a class Process.
* The WorkerTaskManager will have N instances of this class (one per core) and will create a thread per each one.
* When tasks are available, WorkerTaskManager will call the runCommand method to pass commands to the Process managed by this class
* 
* The ProcesssAssistant can pass a "command" to the Process and wait for the process to do staff.
* The process can then pass commands back to the process assitant to notify things like data is available and 
* finally notify that the process is complete.
* 	 
* If the "process" crashes, ProcessAssistant creates again the process and notify about the failure
* If after some timeout, process has not returned finish, the process assistant kills the process and creates it again, notifying about failure
*/

	
	class SamsonWorker;
	class WorkerTaskItem;
	
	/**
	 Top class of the Process Assitant
	 */
	
	class ProcessAssistant : public ProcessAssistantBase
	{
		pthread_t threadId;

	public:

		WorkerTaskItem *item;		// Item that is currently processing ( to notify commands )
		
		ProcessAssistant( int coreNo, SamsonWorker* worker) ;
		
		virtual void receiveCommand( network::ProcessMessage p );

		void run();
	
	};
}

#endif
