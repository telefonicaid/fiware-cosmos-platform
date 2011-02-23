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
#include "Process.h"            // ss::Process



/* ****************************************************************************
*
* starterListInit - 
*/
extern void starterListInit(unsigned int starters);



/* ****************************************************************************
*
* starterAdd - 
*/
extern Starter* starterAdd(Starter* starter);
extern Starter* starterAdd(ss::Process* process);



/* ****************************************************************************
*
* starterRemove - 
*/
extern void starterRemove(Starter* starterP);



/* ****************************************************************************
*
* starterLookup - 
*/
extern Starter* starterLookup(ss::Process* process);
extern Starter* starterLookup(ss::Endpoint* ep);



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

#endif
