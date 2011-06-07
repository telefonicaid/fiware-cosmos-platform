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
	LmtMsg              = 19,

	LmtEndpointListShow = 20,
	LmtStarterListShow,
	LmtSpawnerListShow,
	LmtProcessListShow,

	LmtInit            = 30,
	LmtConfigFile,
	LmtDie,
	LmtHello,
	LmtTimeout,
	LmtMsgTreat,
	LmtHost,

	LmtWorker          = 40,
	LmtWorkers,
	
	LmtEndpoint        = 50,
	LmtEndpoints,
	LmtEndpointUpdate,
	LmtEndpointSlots,
	LmtEndpointAdd,
	LmtTemporalEndpoint,
	LmtIdIx,
	LmtUnhelloed,

	LmtSpawnerList       = 60,
	LmtProcessList,
	LmtStarterList,
	LmtProcessVector,

	LmtStarter          = 70,
	LmtSpawner,
	LmtProcess,
	LmtSetup,

	LmtSpawnerConnect    = 80,
	LmtProcessConnect,
	LmtProcessStart,
	LmtProcessKill,

	LmtEndpointLookup       = 90,
	LmtStarterLookup,
	LmtProcessLookup,
	LmtSpawnerLookup,

	LmtOpen               = 100,
	LmtConnect,
	LmtAccept,
	LmtRead,
	LmtWrite,
	LmtSelect,
	LmtReconnect,

	LmtFds                  = 110,
	LmtNetworkReady,
	LmtAlarm,
	LmtThreads,
	LmtSenderThread,
	LmtThreadedMsgTreat,
	LmtMsgLoopBack,
	LmtControllerConnect,
	LmtPacketReceive,

	LmtNetworkInterface          = 120,
	LmtDelilah,
	LmtDelilahId,
	LmtLogServer,
	LmtSpawn,

	LmtProcessListTab            = 130,
	LmtQtTimer,

	LmtCheck                     = 140,
	LmtFile,
	LmtTask,
	LmtJob,
	LmtSend,
	LmtReceive,                 

	LmtPopupMenu                = 150,
	LmtQueue,
	LmtSource,
	LmtResult,
	LmtConnection,
	LmtQueueMgr,
	LmtSourceMgr,
	LmtResultMgr,

	LmtMouseEvent             = 160,
	LmtUser,

	LmtSceneItemChain         = 170,
	LmtMove,
	LmtWait,

	LmtFree                   = 180,

	LmtEngine                 = 200, // SamsonEngine
	LmtEngineNotification,
	LmtDisk,                         // disk library	
	LmtMemory,                       // memory library	
	LmtProcessManager,

	LmtModuleManager,                // Module Manager component
	LmtOperations,                   // Operations organization (map & reduce level)
	LmtReset,
	LmtMe,
	LmtStructPadding,
	LmtIsolated,
	
    LmtNodeMessages          = 220, 
    
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
