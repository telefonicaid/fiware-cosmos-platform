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
extern const char* processTypeName(ss::ProcessType type);



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
	const char*      host,
	unsigned short   port,
	const char*      alias,
	ss::Endpoint*    endpoint
);



/* ****************************************************************************
*
* spawnerAdd - 
*/
extern ss::Process* spawnerAdd(const char* nameP, const char* host, unsigned short port, ss::Endpoint* endpoint = NULL);



/* ****************************************************************************
*
* processRemove - 
*/
extern void processRemove(ss::Process* processP);



/* ****************************************************************************
*
* processLookup - 
*/
extern ss::Process* processLookup(unsigned int ix);
extern ss::Process* processLookup(const char* alias);
extern ss::Process* processLookup(const char* name, const char* host);
extern ss::Process* processLookup(ss::Endpoint* ep);
extern ss::Process* spawnerLookup(const char* host);



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

#endif
