#ifndef TRACE_LEVELS_H
#define TRACE_LEVELS_H

/* ****************************************************************************
*
* FILE                     traceLevels.h - trace levels for entire Samson project
*
* AUTHOR                   Ken Zangelin
*
* CREATION DATE            Jan 25 2011
*
*/


/* ****************************************************************************
*
* TraceLevels - 
*/
typedef enum TraceLevels
{
//
// Network Trace Levels 20-80
//
	LmtInit = 20,

	LmtOpen,
	LmtConnect,
	LmtRead,
	LmtWrite,
	LmtSelect,

	LmtFds,
	LmtNetworkReady,
	LmtSharedMemory,
	LmtHello,
	LmtDie,
	LmtAlarm,

	LmtSenderThread,
	LmtThreadedMsgTreat,
	LmtMsgLoopBack,
	LmtControllerConnect,

	LmtEndpoint,
	LmtEndpoints,
	LmtEndpointUpdate,
	LmtTemporalEndpoint,

	LmtEndpointListShow,
	LmtStarterListShow,
	LmtSpawnerListShow,
	LmtProcessListShow,
	LmtLogProviderListShow,

	LmtEndpointLookup,
	LmtStarterLookup,
	LmtProcessLookup,
	LmtSpawnerLookup,

	LmtLogProviderList,
	LmtSpawnerList,
	LmtProcessList,

	LmtWorker,
	LmtWorkers,
	LmtWorkerVector,

	LmtLogServer,
	LmtSpawn,
	LmtLogProvider,
	
	LmtSpawnerConnect,
	LmtProcessConnect,
	LmtProcessStart,
	LmtProcessKill,

	LmtStarterList,
	LmtStarter,
	LmtSpawner,
	LmtProcess,

	LmtTask,

	LmtProcessListTab,
	LmtQtTimer,

	LmtCheck,




	LMT_NWRUN,
	LMT_SELECT,
	LMT_ACCEPT,
	LMT_ENDPOINTS,
	LMT_READ,
	LMT_WORKERS,
	LMT_WRITE,
	LMT_MSG,

	LMT_INIT_EP,
	LMT_CONFIG,
	LMT_HELLO,
	LMT_TREAT,
	LMT_RECONNECT,
	LMT_DELILAH,
	LMT_STAT,
	LMT_EP,
	LMT_RESTART,
	LMT_COREWORKER,

	LMT_MSGTREAT,
	LMT_JOB,
	LMT_TIMEOUT,
	LMT_FORWARD,
	LMT_SEND,
	LMT_PA,
	LMT_FILE,
	LMT_SAMSON_WORKER,
	LMT_WINIT,
	LMT_SAMSON_DEMO,
	LMT_CONFIG_FILE,
	LMT_QT
} TraceLevels;



/* ****************************************************************************
*
* traceLevelName - 
*/
extern char* traceLevelName(TraceLevels level);

#endif
