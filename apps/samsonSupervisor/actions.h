#ifndef ACTIONS_H
#define ACTIONS_H

/* ****************************************************************************
*
* FILE                     actions.h
*
* AUTHOR                   Ken Zangelin
*
* CREATION DATE            Jan 13 2011
*
*/
#include "Starter.h"            // Starter
#include "Process.h"            // Process



/* ****************************************************************************
*
* help - 
*/
extern void help(void);



/* ****************************************************************************
*
* list - 
*/
extern void list(void);



/* ****************************************************************************
*
* start - 
*/
extern void start(void);



/* ****************************************************************************
*
* connectToAllSpawners - 
*/
extern void connectToAllSpawners(void);



/* ****************************************************************************
*
* connectToController - 
*/
extern void connectToController(void);



/* ****************************************************************************
*
* processConnect - connect to process
*/
extern void processConnect(Process* processP);



/* ****************************************************************************
*
* connectToAllProcesses - 
*/
extern void connectToAllProcesses(void);



/* ****************************************************************************
*
* startAllProcesses - 
*/
extern void startAllProcesses(void);



/* ****************************************************************************
*
* processStart - start to process
*/
extern void processStart(Process* process, Starter* starter);



/* ****************************************************************************
*
* processKill - kill a process
*/
extern void processKill(Process* process, Starter* starter);

#endif
