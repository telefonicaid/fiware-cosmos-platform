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
	char             name[32];
	char             alias[32];
	char             host[32];
	unsigned short   port;
	int              state;
	bool             verbose;
	bool             debug;
	bool             reads;
	bool             writes;
	bool             toDo;
	char             traceLevels[256];
	bool             sendsLogs;

	ss::Endpoint*    endpoint;
	ProcessType      type;
	Starter*         starterP;     // QT stuff
	struct Process*  spawnerP;     // The process to contact to start me
} Process;

#endif
