#ifndef HOST_H
#define HOST_H

/* ****************************************************************************
*
* FILE                     Host.h
*
* AUTHOR                   Ken Zangelin
*
* CREATION DATE            Feb 16 2011
*
*/



/* ****************************************************************************
*
* Host - 
*/
typedef struct Host
{
	char* name;
	char* ip;
	char* alias[10];
} Host;

#endif
