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
#include <sys/types.h>          // pid_t
#include <sys/time.h>           // struct timeval



/* ****************************************************************************
*
* Forward class declarations
*/
class Starter;



namespace ss
{



/* ****************************************************************************
*
* Forward class declarations
*/
class Endpoint;



/* ****************************************************************************
*
* ProcessType
*/
typedef enum ProcessType
{
	PtWorker      = 1,
	PtController  = 2
} ProcessType;



/* ****************************************************************************
*
* Process - 
*
* All pointers must be the last in the structure, otherwise it wint work in a
* 32-bit machine when sending the message ...
* The pointers aren't interesting on the other side 
*/
typedef struct Process
{
	char                 name[32];
	char                 alias[32];
	char                 host[32];
	unsigned short       port;
	int                  state;
	bool                 verbose;
	bool                 debug;
	bool                 reads;
	bool                 writes;
	bool                 hidden;
	bool                 toDo;
	char                 traceLevels[256];
	bool                 sendsLogs;
	int                  workers;

	char                 controllerHost[32];  // For Supervisor and Spawner

	pid_t                pid;                 // For Spawner
	struct timeval       startTime;           // For Spawner

	ss::ProcessType      type;                // For Spawner and ...
	ss::Endpoint*        endpoint;            // For ...
	Starter*             starterP;            // For Supervisor only
	struct ss::Process*  spawnerP;            // For Supervisor only

#ifndef __LP64__
	int                 padding[6];            // To make this struct the same size in 32 and 64 bit machines
#endif
} Process;



/* ****************************************************************************
*
* ProcessVector
*/
typedef struct ProcessVector
{
	int      processes;
	int      processVecSize;
	Process  processV[];
} ProcessVector;

}

#endif
