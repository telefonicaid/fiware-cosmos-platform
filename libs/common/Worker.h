#ifndef WORKER_H
#define WORKER_H

/* ****************************************************************************
*
* FILE                     Worker.h
*
* AUTHOR                   Ken Zangelin
*
* CREATION DATE            Jan 07 2011
*
*/



namespace ss {



/* ****************************************************************************
*
* Worker
*/
typedef struct Worker
{
	char             name[32];
	char             alias[32];
	char             ip[32];
	int              port;
	int              state;
	bool             verbose;
	bool             debug;
	bool             reads;
	bool             writes;
	bool             toDo;
	char             traceV[256];
} Worker;



/* ****************************************************************************
*
* WorkerVectorData - 
*/
typedef struct WorkerVectorData
{
	int      workers;
	Worker   workerV[0];
} WorkerVectorData;

}
#endif
