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

	LmtInit,
	LmtConfigFile,
	LmtDie,
	LmtHello,
	LmtTimeout,
	LmtMsgTreat,

	LmtWorker,
	LmtWorkers,
	
	LmtEndpoint,
	LmtEndpoints,
	LmtEndpointUpdate,
	LmtEndpointSlots,
	LmtTemporalEndpoint,

	LmtSpawnerList,
	LmtProcessList,
	LmtStarterList,
	LmtProcessVector,

	LmtStarter,
	LmtSpawner,
	LmtProcess,

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
	LmtAlarm,
	LmtSenderThread,
	LmtThreadedMsgTreat,
	LmtMsgLoopBack,
	LmtControllerConnect,

	LmtNetworkInterface,
	LmtDelilah,
	LmtLogServer,
	LmtSpawn,

	LmtIsolated,

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

	LmtSceneItemChain,
	LmtMove,
	LmtWait,

	LmtEngine=200,					// SamsonEngine
	LmtDisk=201,					// disk library	
	LmtMemory=202,					// memory library	

	LmtModuleManager=210,			// Module Manager component
	LmtOperations=211,				// Operations organization (map & reduce level)
	LmtReset,

	
	LmtUser01 = 250,
	LmtUser02 = 251,
	LmtUser03 = 252,
	LmtUser04 = 253,
	LmtUser05 = 254,
	LmtUser06 = 255,
} TraceLevels;



/* ****************************************************************************
*
* traceLevelName - 
*/
extern char* traceLevelName(TraceLevels level);

#endif
