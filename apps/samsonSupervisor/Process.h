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



/* ****************************************************************************
*
* Host - 
*/
typedef struct Spawner
{
	char*            host;
	unsigned short   port;
	int              fd;
} Spawner;



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
* spawnerAdd - 
*/
extern Spawner* spawnerAdd(char* host, unsigned short port, int fd);



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
* spawnerGet - 
*/
extern Spawner* spawnerGet(char* host);



/* ****************************************************************************
*
* processList - 
*/
extern void processList(void);



/* ****************************************************************************
*
* spawnerList - 
*/
extern void spawnerList(void);

#endif
