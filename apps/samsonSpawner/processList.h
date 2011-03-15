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

#include "Process.h"            // ss::Process



/* ****************************************************************************
*
* processListInit - 
*/
extern void processListInit(unsigned int pMax);



/* ****************************************************************************
*
* processTypeName - 
*/
extern const char* processTypeName(ss::Process* processP);



/* ****************************************************************************
*
* processAdd - 
*/
extern ss::Process* processAdd(ss::Process* process);



/* ****************************************************************************
*
* processAdd - 
*/
extern ss::Process* processAdd
(
	ss::ProcessType  type,
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
extern void processRemove(ss::Process* processP);



/* ****************************************************************************
*
* processLookup - 
*/
extern ss::Process* processLookup(pid_t pid);
extern ss::Process* processLookup(const char* alias);



/* ****************************************************************************
*
* processMaxGet - 
*/
extern unsigned int processMaxGet(void);



/* ****************************************************************************
*
* processListGet - 
*/
extern ss::Process** processListGet(void);



/* ****************************************************************************
*
* processListShow - 
*/
extern void processListShow(const char* why, bool forcedOn = false);



/* ****************************************************************************
*
* processSpawn - 
*/
extern void processSpawn(ss::Process* processP);



/* ****************************************************************************
*
* processListDelete - 
*/
extern void processListDelete(void);

#endif
