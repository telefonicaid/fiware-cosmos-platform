#ifndef PROCESS_H
#define PROCESS_H

/* ****************************************************************************
*
* FILE                     Process.h
*
* AUTHOR                   Ken Zangelin
*
* CREATION DATE            Jan 07 2011
*
*/
#include "Spawner.h"            // Spawner



/* ****************************************************************************
*
* Process - 
*/
typedef struct Process
{
	char*            name;
	char*            host;      // hostname can also be obtained in spawner
	Spawner*         spawner;
	int              argCount;
	char*            arg[20];
} Process;



/* ****************************************************************************
*
* processInit - 
*/
extern void processInit(void);



/* ****************************************************************************
*
* processAdd - 
*/
extern Process* processAdd(char* name, char* host, char** args, int argCount);



/* ****************************************************************************
*
* processGet - 
*/
extern Process* processGet(unsigned int ix);



/* ****************************************************************************
*
* processList - 
*/
extern void processList(void);



/* ****************************************************************************
*
* processListGet - 
*/
extern Process** processListGet(unsigned int* noOfP);



/* ****************************************************************************
*
* processesMax -
*/
extern int processesMax(void);

#endif
