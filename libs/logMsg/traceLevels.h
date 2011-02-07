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
	LmtEndpointListShow = 20,
	LmtStarterListShow,
	LmtSpawnerListShow,
	LmtProcessListShow,
	LmtLogProviderListShow,

	LmtInit,
	LmtConfigFile,
	LmtDie,
	LmtHello,
	LmtTimeout,
	LmtMsgTreat,

	LmtWorker,
	LmtWorkers,
	LmtWorkerVector,

	LmtEndpoint,
	LmtEndpoints,
	LmtEndpointUpdate,
	LmtTemporalEndpoint,

	LmtSpawnerList,
	LmtProcessList,
	LmtStarterList,
	LmtLogProviderList,

	LmtStarter,
	LmtSpawner,
	LmtProcess,
	LmtLogProvider,

	LmtSpawnerConnect,
	LmtProcessConnect,
	LmtProcessStart,
	LmtProcessKill,

	LmtEndpointLookup,
	LmtStarterLookup,
	LmtProcessLookup,
	LmtSpawnerLookup,

	LmtOpen,
	LmtConnect,
	LmtAccept,
	LmtRead,
	LmtWrite,
	LmtSelect,
	LmtReconnect,

	LmtFds,
	LmtNetworkReady,
	LmtSharedMemory,
	LmtAlarm,
	LmtSenderThread,
	LmtThreadedMsgTreat,
	LmtMsgLoopBack,
	LmtControllerConnect,

	LmtNetworkInterface,
	LmtDelilah,
	LmtLogServer,
	LmtSpawn,

	LmtProcessListTab,
	LmtQtTimer,

	LmtCheck,
	LmtFile,
	LmtTask,
	LmtJob,
	LmtSend,

	LmtPopupMenu,
	LmtQueue,
	LmtSource,
	LmtResult,
	LmtConnection,
	LmtQueueMgr,
	LmtSourceMgr,
	LmtResultMgr,

	LmtMouseEvent,
	LmtUser,

	LmtSceneItemChain
} TraceLevels;



/* ****************************************************************************
*
* traceLevelName - 
*/
extern char* traceLevelName(TraceLevels level);

#endif
