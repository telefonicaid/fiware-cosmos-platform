
#ifndef _H_PROCESS_ASSISTANT
#define _H_PROCESS_ASSISTANT

namespace ss {
	
	/**
	 
	 Class that is in charge of managing an independent process running a class Process.
	 The WorkerTaskManager will have N instances of this class (one per core) and will create a thread per each one.
	 When tasks are available, WorkerTaskManager will call the runCommand method to pass commands to the Process managed by this class

	 The ProcesssAssistant can pass a "command" to the Process and wait for the process to do staff.
	 The process can then pass commands back to the process assitant to notify things like data is available and 
	 finally notify that the process is complete.
	 
	 If the "process" crashs, ProcessAssistant creates again the process and notify about the failure
	 If after some timeout, process has not returned finish, the process assitnat kills the process, and creates again notiying about failure
	 */	 
	
	class ProcessAssistant{
	
		
		ProcessAssistant()
		{
			// Should create a separate process with a class Process running on it
			// There should be a pipe connecting both the ProcessAssitant and the Process
		}

		
		/*
		 This should send a command to the Process and wait for commands back
		 This method will be called by TaskManager

		 This is suppoused to be a blocking method until one of two thing happend
		 The finish command is received from the Process
		 An error: Process crasshes, report error or timeout is triggered.

		 Return false if an error is found ( any error )
		 */
		 
		bool runCommand( std::string , int timeOut )
		{
		}
		
		/**
		 This command should be executed when a message is received from the Process
		 */
		
		void receivedCommand( std::string c )
		{
			if( c == "finish" )
			{
				// Unblock runCommand returning true ( success )
			} else if( c == "error" )
			{
				// Unblock runCommand returning false ( error )
			}
		
			// TODO: See the command of the child to create packets to be sent to the Network Interface
			
			
		}
		

		
	};
	
}


#endif