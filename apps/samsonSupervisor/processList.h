#ifndef PROCESS_LIST_H
#define PROCESS_LIST_H

/* ****************************************************************************
*
* FILE                     processList.h
*
* AUTHOR                   Ken Zangelin
*
* CREATION DATE            Jan 18 2011
*
*/
#include "Endpoint.h"           // Endpoint
#include "Process.h"            // Process



/* ****************************************************************************
*
* processListInit - 
*/
extern void processListInit(unsigned int pMax);



/* ****************************************************************************
*
* processTypeName - 
*/
extern const char* processTypeName(Process* processP);
extern const char* processTypeName(ProcessType type);



/* ****************************************************************************
*
* processAdd - 
*/
extern Process* processAdd(Process* process);



/* ****************************************************************************
*
* processAdd - 
*/
extern Process* processAdd
(
	const char*     name,
	const char*     host,
	unsigned short  port,
	const char*     alias,
	ss::Endpoint*   endpoint,
	char**          args     = NULL,
	int             argCount = 0
);



/* ****************************************************************************
*
* spawnerAdd - 
*/
extern Process* spawnerAdd(const char* nameP, const char* host, unsigned short port, ss::Endpoint* endpoint = NULL);



/* ****************************************************************************
*
* processLookup - 
*/
extern Process* processLookup(unsigned int ix);
extern Process* processLookup(const char* alias);
extern Process* processLookup(const char* name, const char* host);
extern Process* spawnerLookup(const char* host);



/* ****************************************************************************
*
* processMaxGet - 
*/
extern unsigned int processMaxGet(void);



/* ****************************************************************************
*
* processListGet - 
*/
extern Process** processListGet(void);



/* ****************************************************************************
*
* processListShow - 
*/
extern void processListShow(const char* why);

#endif
