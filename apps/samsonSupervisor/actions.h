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
#include "Spawner.h"            // Spawner
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
* spawnerConnect - connect to spawner
*/
extern void spawnerConnect(Spawner* spawner);



/* ****************************************************************************
*
* spawnerDisconnect - disconnect from spawner
*/
extern void spawnerDisconnect(Spawner* spawner);



/* ****************************************************************************
*
* startAllProcesses - 
*/
extern void startAllProcesses(void);



/* ****************************************************************************
*
* processStart - start to process
*/
extern void processStart(Process* process);



/* ****************************************************************************
*
* processKill - kill a process
*/
extern void processKill(Process* process);

#endif
