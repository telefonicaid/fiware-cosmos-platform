#ifndef SPAWNER_H
#define SPAWNER_H

/* ****************************************************************************
*
* FILE                     Spawner.h
*
* AUTHOR                   Ken Zangelin
*
* CREATION DATE            Jan 13 2011
*
*/



/* ****************************************************************************
*
* Spawner - 
*/
typedef struct Spawner
{
	char*            host;
	unsigned short   port;
	int              fd;
} Spawner;

#endif
