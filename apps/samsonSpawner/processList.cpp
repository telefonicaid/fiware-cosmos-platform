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

#include "logMsg.h"             // LM_*
#include "traceLevels.h"        // Trace Levels
#include "Process.h"            // ss::Process
#include "processList.h"        // Own interface



/* ****************************************************************************
*
* static global vars
*/
static ss::Process**  processV    = NULL;
static unsigned int   processMax  = 0;



/* ****************************************************************************
*
* processListInit - 
*/
void processListInit(unsigned int pMax)
{
	processMax = pMax;
	processV   = (ss::Process**) calloc(processMax, sizeof(ss::Process*));
}



/* ****************************************************************************
*
* processTypeName - 
*/
const char* processTypeName(ss::Process* processP)
{
	switch (processP->type)
	{
	case ss::PtWorker:       return "WorkerStarter";
	case ss::PtController:   return "ControllerStarter";
	case ss::PtSpawner:      return "Spawner";
	}

	return "Unknown Process Type";
}



/* ****************************************************************************
*
* processAdd - 
*/
ss::Process* processAdd(ss::Process* processP)
{
	ss::Process*  pP;
	unsigned int  ix;

	LM_T(LmtProcessList, ("Adding process '%s' of type '%s'. pid: %d", processP->name, processTypeName(processP), processP->pid));

	if ((pP = processLookup(processP->pid)) != NULL)
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
ss::Process* processAdd
(
	ss::ProcessType  type,
	const char*      name,
	const char*      alias,
	const char*      controllerHost,
	pid_t            pid,
	struct timeval*  now
)
{
	ss::Process*   processP;
	struct timeval tv;
 
	if (type == ss::PtWorker)
		LM_T(LmtProcess, ("Adding a Worker process"));
	else if (type == ss::PtController)
		LM_T(LmtProcess, ("Adding a Controller process"));
	else
		LM_X(1, ("Bad process type"));

	if ((processP = processLookup(pid)) != NULL)
	{
		LM_W(("There is already a '%s' with pid %d", name, pid));
		return processP;
	}

	if (name == NULL)
		LM_X(1, ("NULL name for process - find bug, fix it, and recompile!"));

	if (alias == NULL)
		LM_X(1, ("NULL alias for process - find bug, fix it, and recompile!"));

	processP = (ss::Process*) calloc(1, sizeof(ss::Process));
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
ss::Process* processLookup(pid_t pid)
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
ss::Process** processListGet(void)
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
void processRemove(ss::Process* processP)
{
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
