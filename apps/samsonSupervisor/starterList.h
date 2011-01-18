#ifndef STARTER_LIST_H
#define STARTER_LIST_H

/* ****************************************************************************
*
* FILE                     starterList.h
*
* AUTHOR                   Ken Zangelin
*
* CREATION DATE            Jan 18 2011
*
*/
#include "Starter.h"            // Starter
#include "Spawner.h"            // Spawner
#include "Process.h"            // Process



/* ****************************************************************************
*
* starterListInit - 
*/
extern void starterListInit(unsigned int starters);



/* ****************************************************************************
*
* starterMaxGet - 
*/
extern unsigned int starterMaxGet(void);



/* ****************************************************************************
*
* starterListGet - 
*/
extern Starter** starterListGet(void);



/* ****************************************************************************
*
* starterListShow - 
*/
extern void starterListShow(const char* what);



/* ****************************************************************************
*
* starterAdd - 
*/
extern Starter* starterAdd(Starter* starter);
extern Starter* starterAdd(Spawner* spawner);
extern Starter* starterAdd(Process* process);



/* ****************************************************************************
*
* starterLookup - 
*/
extern Starter* starterLookup(ss::Endpoint* ep);



/* ****************************************************************************
*
* starterLookup - 
*/
extern Starter* starterLookup(Process* process);



/* ****************************************************************************
*
* starterLookup - 
*/
extern Starter* starterLookup(Spawner* spawner);

#endif
