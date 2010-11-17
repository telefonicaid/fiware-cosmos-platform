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
#include <string>                  // std::string
#include <pthread.h>               // pthread_t



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
class ProcessAssistant
{
public:
	ProcessAssistant(int coreNo, const char* _controller, SamsonWorker* worker);

private:
	int                            core;
	pthread_t                      threadId;
	time_t                         startTime;
	char*                          controller;
	SamsonWorker*                  worker;
	
	void         coreWorkerStart(char* fatherName, int* rFdP, int* wFdP);

	
	friend class ProcessWriter;
	
	std::string status;	// Internal state to be acess with getStatus

	void setStatus( std::string txt )	// Internal function to set the status
	{
		std::ostringstream s;
		s << "Core " << core << " " << txt;
		status = s.str();
	}
	
	
public:
	void         run(void);
	char*        runCommand(int rFd, int wFd, char* command, int timeOut);
	
	std::string getStatus();

	
};

}

#endif
