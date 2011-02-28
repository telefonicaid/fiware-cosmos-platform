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



namespace ss
{

class Endpoint;


/* ****************************************************************************
*
* ProcessType
*/
typedef enum ProcessType
{
	PtWorker = 1,
	PtController,
	PtSpawner
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
	char                 controllerHost[32];

	ss::ProcessType      type;
	ss::Endpoint*        endpoint;
	Starter*             starterP;     // QT stuff
	struct ss::Process*  spawnerP;     // The process to contact to start me
} Process;

}

#endif
