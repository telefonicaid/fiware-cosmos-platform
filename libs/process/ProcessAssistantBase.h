#ifndef _H_PROCESS_ASSISTANT_BASE
#define _H_PROCESS_ASSISTANT_BASE

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
#include <sstream>					// std::stringstream
#include <iostream>					// std::cout
#include "samson.pb.h"				// ss::network::...
#include "ProcessCommunication.h"		// ss::ProcessCommunication
#include "logMsg.h"						// Log


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
	
	
	class WorkerTaskManager;
	class ProcessAssistantOperationFramework;
	
	/**
	 Base class of the Process Assistant
	 It provides the basic functionality to communicate with the isolated "Process"
	 */
	
	class ProcessAssistantBase : public ProcessCommunication
	{
		
	public:
		
		int core;			// Core for debugging
		int output_shm;		// Identifier for the output shared memory

		WorkerTaskManager* taskManager;	// Pointer to task manager to get new tasks
		
		// functions to start a differnt process / thread to do the job
		void coreWorkerStart();
		void runAsThread();
		void runAsIndependentProcess();
		
		
		// Pipes used between Process and ProcessAssitant
		int pipeFdPair1[2];
		int pipeFdPair2[2];
		
		// Children file descriptors ( thread or process )
		int _rFd;	
		int _wFd;
		
		
	public:
		
		// Constructor
		ProcessAssistantBase( int _core, WorkerTaskManager* _taskManager );
		
		// Send a command to the Process
		network::ProcessMessage runCommand( network::ProcessMessage p );
		
		// Function that should be implemented in top classes to receive asyn notifications from the Process
		virtual void receiveCommand( network::ProcessMessage p )=0;		 
		
	public:
		
		/**
		 Status management
		 */
		
		std::string status;					// Internal state to be acess with getStatus
		void setStatus( std::string txt )	// Internal function to set the status
		{
			std::ostringstream s;
			s << "Core " << core << " [Output shm: " << output_shm << "] "  << txt;
			status = s.str();
		}	
		
		std::string getStatus()
		{
			return status;
		}
		
	};
}
#endif

