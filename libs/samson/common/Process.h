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



namespace samson
{


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
	samson::ProcessType      type;
	int                      id;
	char                     name[64];
	char                     alias[64];
	char                     host[64];
	unsigned short           port;
	unsigned short           pad01;
	int                      state;
	int                      pad02;
	bool                     verbose;
	bool                     debug;
	bool                     reads;
	bool                     writes;
	bool                     hidden;
	bool                     toDo;
	char                     traceLevels[256];
	bool                     sendsLogs;
	int                      workers;
	char                     controllerHost[32];  // For Supervisor and Spawner
	

	struct timeval           startTime;           // For Spawner
	pid_t                    pid;                 // For Spawner

	Starter*                 starterP;            // For Supervisor only
	struct samson::Process*  spawnerP;            // For Supervisor only

#ifndef __LP64__
	int                      pad03[6];            // three pointers - to make this struct the same size in 32 and 64 bit machines
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
