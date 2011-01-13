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



/* ****************************************************************************
*
* spawnerAdd - 
*/
extern Spawner* spawnerAdd(char* host, unsigned short port, int fd);



/* ****************************************************************************
*
* spawnerGet - 
*/
extern Spawner* spawnerGet(char* host);



/* ****************************************************************************
*
* spawnerList - 
*/
extern void spawnerList(void);

#endif
