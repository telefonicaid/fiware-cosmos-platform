/* ****************************************************************************
*
* FILE                     ProcessAssistant.cpp
*
* AUTHOR                   Ken Zangelin
*
* CREATION DATE            Nov 5 2010
*
*/
#include <time.h>                  // time
#include <pthread.h>               // pthread_t

#include "logMsg.h"                // LM_*
#include "Alarm.h"                 // ALARM
#include "processTraceLevels.h"    // LMT_*

#include "Message.h"               // Message::*
#include "iomServerOpen.h"         // iomServerOpen
#include "iomConnect.h"            // iomConnect
#include "iomAccept.h"             // iomAccept
#include "iomMsgAwait.h"           // iomMsgAwait
#include "iomMsgSend.h"            // iomMsgSend
#include "iomMsgRead.h"            // iomMsgRead
#include "Endpoint.h"              // Endpoint
#include "SamsonWorker.h"          // SamsonWorker
#include "Process.h"               // Process
#include "ProcessAssistant.h"      // Own interface



namespace ss {



/* ****************************************************************************
*
* runThread - 
*/
static void* runThread(void* vP)
{
	ProcessAssistant* paP = (ProcessAssistant*) vP;
	
	paP->run();
	return NULL;
}



/* ****************************************************************************
*
* Constructor
*/
ProcessAssistant::ProcessAssistant(int coreNo, const char* _controller, SamsonWorker* _worker)
{
	core       = coreNo;
	controller = strdup(_controller);
	worker     = _worker;

	pthread_create(&threadId, NULL, runThread, this);
}


extern int logFd;

/* ****************************************************************************
*
* coreWorkerStart - 
*/
void ProcessAssistant::coreWorkerStart(char* fatherName, int* rFdP, int* wFdP)
{
	time_t now = time(NULL);
	int    pipeFd[2];

	if (now - startTime <= 5)
	{
		// ALARM(Alarm::Error, Alarm::CoreWorkerNotRestarted, ("Core worker %d died %d secs after restart", core, now - startTime));
		LM_RVE(("Core worker %d died %d secs after restart", core, now - startTime));
	}

	if (pipe(pipeFd) != 0)  // pipe[0] for reading
		LM_RVE(("pipe: %s", strerror(errno)));

	LM_M(("pipe fds: %d %d", pipeFd[0], pipeFd[1]));
	*rFdP = pipeFd[0];
	*wFdP = pipeFd[1];

	LM_T(LMT_COREWORKER, ("*********** Starting Core Worker %d (father reading on fd %d and writing on fd %d", core, *rFdP, *wFdP));
	if (fork() != 0)
		return;

	int rFd = pipeFd[0];
	int wFd = pipeFd[1];

	LM_T(60, ("CHILD RUNNING (rFd: %d, wFd: %d", rFd, wFd));
	/* ************************************************************
	 *
	 * Setting auxiliar string for logMsg
	 */
	char auxString[16];
	
	sprintf(auxString, "core%02d", core);
	lmAux(auxString);

	LM_T(60, ("CHILD RUNNING (logFd == %d)", logFd));


#if !defined(__APPLE__)
	cpu_set_t cpuSet;

	CPU_ZERO(&cpuSet);
	CPU_SET(core, &cpuSet);
	if (sched_setaffinity(0, sizeof(cpuSet), &cpuSet) == -1)
		LM_XP(1, ("sched_setaffinity"));
#endif
		
	LM_T(LMT_COREWORKER, ("child %d running (pid: %d) on core %d", core, (int) getpid(), core));



	/* ************************************************************
	 *
	 * Core workers do not log to stdout
	 */
	lmFdUnregister(1);


	
	/* ************************************************************
	 *
	 * Close fathers fds ...
	 */
	int fd;
	
	LM_T(60, ("Close fathers file descriptors (but not mine: logFd == %d, rFd: %d, wFd: %d)", logFd, rFd, wFd));
	for (fd = 0; fd < 100; fd++)
	{
		if ((fd != logFd) && (fd != rFd) && (fd != wFd))
			close(fd);
		else
			LM_T(60, ("Not closing fd %d", fd));
	}
	LM_M(("All fathers file descriptors closed!"));



	/* ************************************************************
	 *
	 * Set progName
	 */
	progName = (char*) malloc(strlen("samsonCoreWorker_") + 10);
	if (progName == NULL)
		LM_X(1, ("samsonCoreWorker_%d died allocating: %s", getpid(), strerror(errno)));
	sprintf(progName, (char*) "samsonCoreWorker_%d", (int) getpid());



	LM_T(LMT_COREWORKER, ("Creating Process(rFd:%d, wFd:%d)", rFd, wFd));
	Process* processP = new Process(rFd, wFd);
	if (processP == NULL)
		LM_X(1, ("error allocating a Process"));
			
	processP->run();
	LM_X(1, ("Back from run - should not ever get here (coreWorker %d, pid: %d)", core, (int) getpid()));
}



/* ****************************************************************************
*
* run - 
*/
void ProcessAssistant::run(void)
{
	// TODO: Andreu		   
	//   o Get a new task to do ( or block until ready )		   
	//   o Pass command to Process with "runCommand"
	//   o Loop receiving messages from Process until "finish" or "crash" received
	//   o Send "continue"

	int rFd;     // file descriptor for reading
	int wFd;     // file descriptor for writing

	LM_TODO(("Will connect to controller when ProcessAssistant uses EndpointMgr"));

	// Endpoint* controllerP = worker->controller;



	/* **********************************************************************
	 *
	 * Starting Core Worker
	 */
	startTime = 0;
	coreWorkerStart(progName, &rFd, &wFd);

	while (true)
	{
		char* result;

		// Get the next item to process ( this call is blocked if no task is available )
		WorkerTaskItem *item =  worker->taskManager.getNextItemToProcess();

		LM_T(LMT_COREWORKER, ("Running command '%s' (rFd: %d, wFd: %d)", item->operation.c_str(), rFd, wFd));
		result = runCommand(rFd, wFd, (char*) item->operation.c_str() , 5);

		// Loop receiving command from the Process until "finish" or "crash" received
		while( (strcmp(result, "finish") != 0 ) && (strcmp(result, "crash") != 0) && (strcmp(result, "error") != 0 ) )
		{
			// Do something with the received command ( create a buffer with output data )
			// TODO: pending
			free(result);
			
			result = runCommand(rFd, wFd, (char*) "continue" , 5);
		}
		
		// Report finish of this task
		
		if (strcmp(result, "finish") == 0)
		{
			LM_W(("Got finish from runCommand"));
			worker->taskManager.finishItem(item, false, "");
			result = runCommand(rFd, wFd, (char*) "ok" , 5);
		}
		else if (strcmp(result, "crash") == 0)
		{
			LM_W(("child process crashed - starting a new process"));
			close(rFd);
			close(wFd);

			// ALARM(Alarm::Error, Alarm::CoreWorkerDied, ("Core worker %d died", core));
			worker->taskManager.finishItem(item, true, "Process crashed");
			coreWorkerStart(progName, &rFd, &wFd);
		}
		else if (strcmp(result, "error") == 0)
		{
		  LM_W(("child process error - starting a new process"));
		  // ALARM(Alarm::Error, Alarm::CoreWorkerDied, ("Core worker %d died", core));
		  worker->taskManager.finishItem(item, true, "Process error");
		}
			  
		free(result);
	}
}



/* ****************************************************************************
*
* runCommand
*
* The method 'runCommand' sends a command to the Process and waits for commands back.
* This method is called by TaskManager.
* 
* This is supposed to be a blocking method until one of two thing happens:
* - The 'finish' command is received from the Process
* - An error occurs: process crashes, reports error or a timeout is triggered.
* 
* RETURN VALUE
*   true    is returned on success, while
*   false   is returned on error (any error)
*/
char* ProcessAssistant::runCommand(int rFd, int wFd, char* command, int timeOut)
{
	int                   s;
	char                  out[128];
	void*                 dataP   = out;
	int                   dataLen = sizeof(out);
	Message::MessageCode  msgCode;
	Message::MessageType  msgType;
	char*                 result;

	LM_T(60, ("sending command '%s' on fd %d", command, wFd));
	s = iomMsgSend(wFd, "coreWorker", progName, Message::Command, Message::Msg, command, strlen(command) + 1);
	if (s != 0)
		LM_RP(strdup("error"), ("iomMsgSend error"));

	while (1)
	{
		s = iomMsgAwait(rFd, 5, 0);
		if (s == -2)
			return strdup("timeout");
		else if (s != 1)
			LM_RE(strdup("error"), ("iomMsgAwait returned -1"));

		s = iomMsgRead(rFd, "coreWorker", &msgCode, &msgType, &dataP, &dataLen, NULL, NULL, 0);
		if (s == -2)
			LM_RE(strdup("crash"), ("connection closed by core child process 'fd:%d', running the command '%s' ...", rFd, command));
		else if (s != 0)
			LM_RE(strdup("error"), ("iomMsgRead error"));

		return strdup((char*) dataP);
#if 0
		else if (msgCode == Message::Alarm)
		{
			Message::AlarmData* alarmP = (Message::AlarmData*) dataP;

			LM_T(LMT_COREWORKER, ("child sent an alarm - forwarding it to my father"));
		}
		else if (msgCode == Message::Hello)
		{
			Message::HelloData* helloP = (Message::HelloData*) dataP;

			LM_T(LMT_COREWORKER, ("child sent a Hello - keeping info"));
		}
#endif
		if (dataP != out)
			free(dataP);
	}

	return result;
}

}
