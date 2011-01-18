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
#include "Process.h"            // Process
#include "Spawner.h"            // Spawner



/* ****************************************************************************
*
* processListInit - 
*/
extern void processListInit(unsigned int pMax);



/* ****************************************************************************
*
* processAdd - 
*/
extern Process* processAdd(Process* process);



/* ****************************************************************************
*
* processAdd - 
*/
extern Process* processAdd(char* name, char* host, char** args, int argCount);



/* ****************************************************************************
*
* processLookup - 
*/
extern Process* processLookup(unsigned int ix);



/* ****************************************************************************
*
* processLookup - 
*/
extern Process* processLookup(char* name, char* host);



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
