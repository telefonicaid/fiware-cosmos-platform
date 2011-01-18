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
* Forward class declarations
*/
class Starter;
struct Spawner;



/* ****************************************************************************
*
* Process - 
*/
typedef struct Process
{
	char*            name;
	char*            host;      // hostname can also be obtained in spawner
	int              argCount;
	char*            arg[20];

	struct Spawner*  spawnerP;
	Starter*         starterP;
} Process;

#endif
