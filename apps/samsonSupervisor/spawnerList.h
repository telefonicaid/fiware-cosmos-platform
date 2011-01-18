#ifndef SPAWNER_LIST_H
#define SPAWNER_LIST_H

/* ****************************************************************************
*
* FILE                     spawnerList.h
*
* AUTHOR                   Ken Zangelin
*
* CREATION DATE            Jan 18 2011
*
*/
#include "Spawner.h"            // Spawner



/* ****************************************************************************
*
* spawnerListInit - 
*/
extern void spawnerListInit(unsigned int sMax);



/* ****************************************************************************
*
* spawnerAdd - 
*/
extern Spawner* spawnerAdd(Spawner* spawner);



/* ****************************************************************************
*
* spawnerAdd - 
*/
extern Spawner* spawnerAdd(char* host, unsigned short port, int fd);



/* ****************************************************************************
*
* spawnerLookup - 
*/
extern Spawner* spawnerLookup(unsigned int ix);



/* ****************************************************************************
*
* spawnerLookup - 
*/
extern Spawner* spawnerLookup(char* host);



/* ****************************************************************************
*
* spawnerMaxGet - 
*/
extern unsigned int spawnerMaxGet(void);



/* ****************************************************************************
*
* spawnerListGet - 
*/
extern Spawner** spawnerListGet(void);



/* ****************************************************************************
*
* spawnerListShow - 
*/
extern void spawnerListShow(const char* why);

#endif
