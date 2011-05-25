#ifndef PROCESS_LIST_H
#define PROCESS_LIST_H

/* ****************************************************************************
*
* FILE                     processList.h
*
* AUTHOR                   Ken Zangelin
*
* CREATION DATE            Mar 01 2011
*
*/
#include <sys/time.h>           // struct timeval

#include "samson/common/Process.h"            // samson::Process



/* ****************************************************************************
*
* processListInit - 
*/
extern void processListInit(unsigned int pMax);



/* ****************************************************************************
*
* processTypeName - 
*/
extern const char* processTypeName(samson::Process* processP);



/* ****************************************************************************
*
* processAdd - 
*/
extern samson::Process* processAdd(samson::Process* process);



/* ****************************************************************************
*
* processAdd - 
*/
extern samson::Process* processAdd
(
	samson::ProcessType  type,
	const char*      name,
	const char*      alias,
	const char*      controllerHost,
	pid_t            pid,	
	struct timeval*  now
);



/* ****************************************************************************
*
* processRemove - 
*/
extern void processRemove(samson::Process* processP);



/* ****************************************************************************
*
* processLookup - 
*/
extern samson::Process* processLookup(pid_t pid);
extern samson::Process* processLookup(const char* alias);



/* ****************************************************************************
*
* processMaxGet - 
*/
extern unsigned int processMaxGet(void);



/* ****************************************************************************
*
* processListGet - 
*/
extern samson::Process** processListGet(void);



/* ****************************************************************************
*
* processListShow - 
*/
extern void processListShow(const char* why, bool forcedOn = false);



/* ****************************************************************************
*
* processSpawn - 
*/
extern void processSpawn(samson::Process* processP);



/* ****************************************************************************
*
* processListDelete - 
*/
extern void processListDelete(void);

#endif
