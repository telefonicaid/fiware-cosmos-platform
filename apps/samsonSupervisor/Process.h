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
#include "Endpoint.h"           // Endpoint



/* ****************************************************************************
*
* Forward class declarations
*/
class Starter;



/* ****************************************************************************
*
* SpawnInfo
*/
typedef struct SpawnInfo
{
	struct Process*  spawnerP;
	int              argCount;
	char*            arg[20];
} SpawnInfo;



/* ****************************************************************************
*
* ProcessType
*/
typedef enum ProcessType
{
	PtWorkerStarter,
	PtControllerStarter,
	PtSpawner
} ProcessType;



/* ****************************************************************************
*
* Process - 
*/
typedef struct Process
{
	char*            name;
	char*            alias;
	char*            host;      // hostname can also be obtained in spawner
	unsigned short   port;
	ss::Endpoint*    endpoint;
	ProcessType      type;
	Starter*         starterP;
	SpawnInfo*       spawnInfo;
	bool             sendsLogs;
} Process;

#endif
