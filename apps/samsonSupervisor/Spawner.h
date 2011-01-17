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
* spawnerInit - 
*/
extern void spawnerInit(void);



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
* spawnerGet - 
*/
extern Spawner* spawnerGet(unsigned int ix);



/* ****************************************************************************
*
* spawnerList - 
*/
extern void spawnerList(void);



/* ****************************************************************************
*
* spawnerListGet - 
*/
extern Spawner** spawnerListGet(unsigned int* noOfP);



/* ****************************************************************************
*
* spawnersMax - 
*/
extern int spawnersMax(void);

#endif
