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
#include "Process.h"               // Process
#include "ProcessAssistant.h"      // Own interface



namespace ss {



/* ****************************************************************************
*
* definitions
*/
#define PA_PORT 1300



/* ****************************************************************************
*
* global variables
*/
static unsigned short port = PA_PORT;



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
ProcessAssistant::ProcessAssistant(int coreNo)
{
	core = coreNo;

	pthread_create(&threadId, NULL, runThread, this);
}



/* ****************************************************************************
*
* coreWorkerStart - 
*/
void ProcessAssistant::coreWorkerStart(char* fatherName, unsigned short port)
{
	time_t now = time(NULL);

	if (now - startTime <= 5)
	{
		// ALARM(Alarm::Error, Alarm::CoreWorkerNotRestarted, ("Core worker %d died %d secs after restart", core, now - startTime));
		LM_E(("Core worker %d died %d secs after restart", core, now - startTime));
		return;
	}

	LM_T(LMT_COREWORKER, ("*********** Starting Core Worker %d", core));
	if (fork() != 0)
		return;


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
	 * Set progName
	 */
	progName = (char*) malloc(strlen("samsonCoreWorker_") + 10);
	if (progName == NULL)
		LM_X(1, ("samsonCoreWorker_%d died allocating: %s", getpid(), strerror(errno)));
	sprintf(progName, (char*) "samsonCoreWorker_%d", (int) getpid());


		
	/* ************************************************************
	 *
	 * Setting auxiliar string for logMsg
	 */
	char auxString[16];
	
	sprintf(auxString, "child-core%02d", core);
	lmAux(auxString);



	LM_T(LMT_COREWORKER, ("Connecting to father"));
	int  fd;
	fd = iomConnect("localhost", port);
	if (fd == -1)
		LM_X(1, ("error connecting to father at %s:%d", "localhost", port));
	LM_T(LMT_COREWORKER, ("Connected to father"));

	LM_T(LMT_COREWORKER, ("Calling RUN"));
	Process* processP = new Process(fd);
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
	int lFd;     // file descriptor for listen socket
	int sFd;     // file descriptor for socket connection
	int fds;

	/* ************************************************************
	 *
	 * Core workers do not log to stdout
	 */
	lmFdUnregister(1);


	
	/* ************************************************************
	 *
	 * Set progName
	 */
	progName = (char*) malloc(strlen("samsonCoreWorker_") + 10);
	if (progName == NULL)
		LM_X(1, ("samsonCoreWorker_%d died allocating: %s", getpid(), strerror(errno)));
	sprintf(progName, (char*) "samsonCoreWorker_%d", (int) getpid());


		
	/* ************************************************************
	 *
	 * Setting auxiliar string for logMsg
	 */
	char auxString[16];
	
	sprintf(auxString, "father-core%02d", core);
	lmAux(auxString);



	LM_M(("IN: pid: %d", getpid()));

	// TODO: Andreu		   
	//   o Get a new task to do ( or block until ready )		   
	//   o Pass command to Process with "runCommand"
	//   o Loop receiving messages from Process until "finish" or "crash" received
	//   o Send "continue"


	while (1)
	{
		lFd = iomServerOpen(port);
		if (lFd != -1)
			break;
		LM_W((""));
		++port;
	}

	startTime = 0;
	coreWorkerStart(progName, port);
	LM_T(LMT_PA, ("Awaiting connection on port %d", port));
	fds = iomMsgAwait(lFd, 5, 0);
	if (fds != 1)
		LM_X(1, ("core worker did not connect in 5 secs ..."));

	sFd = iomAccept(lFd);
	if (sFd == -1)
		LM_X(1, ("error accepting core worker"));

	while (true)
	{
		char* result;

		LM_T(LMT_PA, ("Sleeping 10 secs"));
		sleep(10);

		LM_T(LMT_PA, ("Running a command"));
		result = runCommand(sFd, (char*) "test", 5);

		if (strcmp(result, "finish") == 0)
			LM_W(("Got finish from runCommand"));
		else if (strcmp(result, "crash") == 0)
		{
			LM_W(("child process crashed - starting a new process"));
			// ALARM(Alarm::Error, Alarm::CoreWorkerDied, ("Core worker %d died", core));
			coreWorkerStart(progName, port);
		}

		free(result);
	}
}



/* ****************************************************************************
*
* runCommand
*
* This method sends a command to the Process and waits for commands back.
* This method will be called by TaskManager.
* 
* This is supposed to be a blocking method until one of two thing happens:
* - The finish command is received from the Process
* - An error: Proces crashes, report error or timeout is triggered.
* 
* RETURN VALUE
* true is returned upon success and 
* false is returned on error (any error)
*/
char* ProcessAssistant::runCommand(int fd, char* command, int timeOut)
{
	int                   s;
	char                  out[128];
	void*                 dataP   = out;
	int                   dataLen = sizeof(out);
	Message::MessageCode  msgCode;
	Message::MessageType  msgType;
	char*                 result;

	s = iomMsgSend(fd, "coreWorker", progName, Message::Command, Message::Msg, command, strlen(command));
	if (s != 0)
		return strdup("error");

	s = iomMsgAwait(fd, 5, 0);
	if (s == -2)
		return strdup("timeout");
	else if (s != 1)
		return strdup("error");

	s = iomMsgRead(fd, "coreWorker", &msgCode, &msgType, &dataP, &dataLen, NULL, NULL, 0);
	if (s == -2)
		result = strdup("crash");
	else if (s != 0)
		result = strdup("error");
	else 
		result = strdup(out);

	if (dataP != out)
		free(dataP);

	return result;
}

}
