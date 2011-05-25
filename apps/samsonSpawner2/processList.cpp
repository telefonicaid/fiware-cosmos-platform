/* ****************************************************************************
*
* FILE                     processList.cpp
*
* AUTHOR                   Ken Zangelin
*
* CREATION DATE            Mar 01 2011
*
*/
#include <string.h>             // memset, strcpy, ...
#include <sys/time.h>           // struct timeval
#include <unistd.h>             // fork & exec
#include <sys/types.h>          // pid_t

#include "logMsg/logMsg.h"             // LM_*
#include "logMsg/traceLevels.h"        // Trace Levels
#include "samson/common/Process.h"            // samson::Process
#include "processList.h"        // Own interface
#include <stdlib.h>             // free()



/* ****************************************************************************
*
* static global vars
*/
static samson::Process**  processV    = NULL;
static unsigned int   processMax  = 0;



/* ****************************************************************************
*
* processListInit - 
*/
void processListInit(unsigned int pMax)
{
	processMax = pMax;
	processV   = (samson::Process**) calloc(processMax, sizeof(samson::Process*));
}



/* ****************************************************************************
*
* processTypeName - 
*/
const char* processTypeName(samson::Process* processP)
{
	switch (processP->type)
	{
	case samson::PtWorker:       return "WorkerStarter";
	case samson::PtController:   return "ControllerStarter";
	}

	return "Unknown Process Type";
}



/* ****************************************************************************
*
* processAdd - 
*/
samson::Process* processAdd(samson::Process* processP)
{
	samson::Process*  pP;
	unsigned int  ix;

	if (processMax == 0)
	   LM_X(1, ("processMax == 0 - processListInit not called !"));

	LM_T(LmtProcessList, ("Adding process '%s' of type '%s'. pid: %d", processP->name, processTypeName(processP), processP->pid));

	if ((processP->pid != 0) && ((pP = processLookup(processP->pid)) != NULL))
	{
		LM_W(("process '%s' (pid %d) already in process list", processP->name, processP->pid));
		free(processP);
		return pP;
	}

	for (ix = 0; ix < processMax; ix++)
	{
		if (processV[ix] == NULL)
			break;
	}

	if (ix >= processMax)
	{
		LM_TODO(("realloc"));
		LM_X(1, ("No room for more Processes (max index is %d) - change and recompile!", processMax));
	}

	processV[ix] = processP;
	LM_T(LmtProcess, ("Added process '%s' type '%s' with pid %d", processP->name, processTypeName(processP), processP->pid));
	processListShow("Process added");

	return processP;
}



/* ****************************************************************************
*
* processAdd - 
*/
samson::Process* processAdd
(
	samson::ProcessType  type,
	const char*      name,
	const char*      alias,
	const char*      controllerHost,
	pid_t            pid,
	struct timeval*  now
)
{
	samson::Process*   processP;
	struct timeval tv;
 
	if (type == samson::PtWorker)
		LM_T(LmtProcess, ("Adding a Worker process"));
	else if (type == samson::PtController)
		LM_T(LmtProcess, ("Adding a Controller process"));
	else
		LM_X(1, ("Bad process type"));

	if ((processP = processLookup(pid)) != NULL)
	{
	   LM_W(("There is already a '%s' (with pid %d)", name, pid));
		return processP;
	}

	if (name == NULL)
		LM_X(1, ("NULL name for process - find bug, fix it, and recompile!"));

	if (alias == NULL)
		LM_X(1, ("NULL alias for process - find bug, fix it, and recompile!"));

	processP = (samson::Process*) calloc(1, sizeof(samson::Process));
	if (processP == NULL)
		LM_X(1, ("calloc: %s", strerror(errno)));

	strncpy(processP->name,           name,           sizeof(processP->name));
	strncpy(processP->alias,          alias,          sizeof(processP->alias));
	strncpy(processP->controllerHost, controllerHost, sizeof(processP->controllerHost));

	processP->type       = type;
	processP->pid        = pid;

	if (now == NULL)
	{
		if (gettimeofday(&tv, NULL) != 0)
			LM_X(1, ("gettimeofday failed (fatal error): %s", strerror(errno)));

		now = &tv;
	}

	processP->startTime.tv_sec  = now->tv_sec;
	processP->startTime.tv_usec = now->tv_usec;

	return processAdd(processP);
}



/* ****************************************************************************
*
* processLookup - 
*/
samson::Process* processLookup(pid_t pid)
{
	for (unsigned int ix = 0; ix < processMax; ix++)
	{
		if (processV[ix] == NULL)
			continue;

		if (processV[ix]->pid == pid)
			return processV[ix];
	}

	return NULL;
}



/* ****************************************************************************
*
* processMaxGet - 
*/
unsigned int processMaxGet(void)
{
	return processMax;
}



/* ****************************************************************************
*
* processListGet - 
*/
samson::Process** processListGet(void)
{
	return processV;
}



/* ****************************************************************************
*
* processListShow - 
*/
void processListShow(const char* why, bool forcedOn)
{
	if (forcedOn)
		LM_F(("---------- Process List: %s ----------", why));
	else
		LM_T(LmtProcessListShow, ("---------- Process List: %s ----------", why));

	for (unsigned int ix = 0; ix < processMax; ix++)
	{
		if (processV[ix] == NULL)
			continue;

		if (forcedOn)
			LM_F(("  process %02d: %-20s %-20s  pid %d",
				  ix,
				  processV[ix]->name,
				  processV[ix]->alias,
				  processV[ix]->pid));
		else
			LM_T(LmtProcessListShow, ("  process %02d: %-20s %-20s  pid %d",
									  ix,
									  processV[ix]->name,
									  processV[ix]->alias,
									  processV[ix]->pid));

	}

	if (forcedOn)
		LM_F(("------------------------------------"));
	else
		LM_T(LmtProcessListShow, ("------------------------------------"));
}



/* ****************************************************************************
*
* processRemove - 
*/
void processRemove(samson::Process* processP)
{
	LM_T(LmtProcessList, ("Removing process '%s'", processP->name));

	for (unsigned int ix = 0; ix < processMax; ix++)
	{
		if (processV[ix] == processP)
		{
			free(processP);
			processV[ix] = NULL;
		}
	}

	processListShow("Removed a process");
}



/* ****************************************************************************
*
* processListDelete - 
*/
void processListDelete(void)
{
	for (unsigned int ix = 0; ix < processMax; ix++)
	{
		if (processV[ix] == NULL)
			continue;

		processRemove(processV[ix]);
	}

	free(processV);
}
