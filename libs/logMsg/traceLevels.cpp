/* ****************************************************************************
*
* FILE                     traceLevels.cpp - trace levels for entire Samson project
*
* AUTHOR                   Ken Zangelin
*
* CREATION DATE            Jan 25 2011
*
*/
#include "traceLevels.h"         // Own interface



/* ****************************************************************************
*
* traceLevelName - 
*/
char* traceLevelName(TraceLevels level)
{
	switch (level)
	{
	case LmtInit:                        return (char*) "Initialization, especially in main";

	case LmtOpen:                        return (char*) "open system calls";
	case LmtConnect:                     return (char*) "socket connect system calls";
	case LmtRead:                        return (char*) "socket/fd read system calls";
	case LmtWrite:                       return (char*) "socket/fd write system calls";
	case LmtSelect:                      return (char*) "select system calls";

	case LmtFds:                         return (char*) "File descriptors";
	case LmtNetworkReady:                return (char*) "Actions on Network module ready";
	case LmtSharedMemory:                return (char*) "Shared Memory";
	case LmtDie:                         return (char*) "Die message sent/received";
	case LmtAlarm:                       return (char*) "At issuing an Alarm";
	case LmtHello:                       return (char*) "Hello message interchange";
	case LmtSenderThread:                return (char*) "Network Sender Thread";
	case LmtThreadedMsgTreat:            return (char*) "Threaded Msg Treat function";
	case LmtMsgLoopBack:                 return (char*) "Network loops back messages for same machine";
	case LmtControllerConnect:           return (char*) "Connection to controller";

	case LmtEndpointListShow:            return (char*) "Show Endpoint List";
	case LmtStarterListShow:             return (char*) "Show Starter List (Supervisor only)";
	case LmtSpawnerListShow:             return (char*) "Show Spawner List (Supervisor only)";
	case LmtProcessListShow:             return (char*) "Show Process List (Supervisor only)";
	case LmtLogProviderListShow:         return (char*) "Show Log Provider List (LogServer only)";

	case LmtWorker:                      return (char*) "Worker";
	case LmtWorkers:                     return (char*) "Worker List";
	case LmtWorkerVector:                return (char*) "Worker Vector";

	case LmtEndpoint:                    return (char*) "Endpoint";
	case LmtEndpoints:                   return (char*) "Endpoints";
	case LmtEndpointUpdate:              return (char*) "Updating an Endpoint";
	case LmtTemporalEndpoint:            return (char*) "Temporal Endpoints";

	case LmtLogServer:                   return (char*) "Log Server";
	case LmtSpawn:                       return (char*) "Spawning processes (Spawner only)";

	case LmtSpawnerList:                 return (char*) "Spawner List (Supervisor only)";
	case LmtProcessList:                 return (char*) "Process List (Supervisor only)";
	case LmtStarterList:                 return (char*) "Starter List (Supervisor only)";

	case LmtStarter:                     return (char*) "Starters (Supervisor only)";
	case LmtSpawner:                     return (char*) "Spawners (Supervisor only)";
	case LmtProcess:                     return (char*) "Processes (Supervisor only)";

	case LmtSpawnerConnect:              return (char*) "Connecting to Spawner";
	case LmtProcessConnect:              return (char*) "Connecting to Process";
	case LmtProcessStart:                return (char*) "Starting a Process";
	case LmtProcessKill:                 return (char*) "Killing a Process";

	case LmtEndpointLookup:              return (char*) "Endpoint Lookup";
	case LmtStarterLookup:               return (char*) "Looking up Starters (Supervisor only)";
	case LmtProcessLookup:               return (char*) "Lookuping up a Process";
	case LmtSpawnerLookup:               return (char*) "Looking up a Spawner";

	case LmtTask:                        return (char*) "Task";

	case LmtProcessListTab:              return (char*) "Process List QT Tab";
	case LmtQtTimer:                     return (char*) "QT timers";

	case LmtCheck:                       return (char*) "Check checkbox state (Supervisor only)";

	case LMT_NWRUN:                      return (char*) "Nwrun";
	case LMT_SELECT:                     return (char*) "Select";
	case LMT_ACCEPT:                     return (char*) "Accept";
	case LMT_ENDPOINTS:                  return (char*) "Endpoints";
	case LMT_READ:                       return (char*) "Read";
	case LMT_WORKERS:                    return (char*) "Workers";
	case LMT_WRITE:                      return (char*) "Write";
	case LMT_MSG:                        return (char*) "Msg";
	case LMT_INIT_EP:                    return (char*) "InitEp";
	case LMT_CONFIG:                     return (char*) "Config";
	case LMT_HELLO:                      return (char*) "Hello";
	case LMT_TREAT:                      return (char*) "Treat";
	case LMT_RECONNECT:                  return (char*) "Reconnect";
	case LMT_DELILAH:                    return (char*) "Delilah";
	case LMT_STAT:                       return (char*) "Stat";
	case LMT_EP:                         return (char*) "Endpoints";
	case LMT_RESTART:                    return (char*) "Restart";
	case LMT_COREWORKER:                 return (char*) "Core Worker";
	case LMT_MSGTREAT:                   return (char*) "Msg Treat";
	case LMT_JOB:                        return (char*) "Job";
	case LMT_TIMEOUT:                    return (char*) "Timeout";
	case LMT_FORWARD:                    return (char*) "Forward";
	case LMT_SEND:                       return (char*) "Send";

	case LMT_PA:                         return (char*) "Pa";
	case LMT_FILE:                       return (char*) "File";
	case LMT_SAMSON_WORKER:              return (char*) "Samson Worker";
	case LMT_WINIT:                      return (char*) "Winit";
	case LmtLogProvider:                 return (char*) "Log Provider";
	case LmtLogProviderList:             return (char*) "Log Provider List";
	case LMT_SAMSON_DEMO:                return (char*) "Samson Demo";
	case LMT_CONFIG_FILE:                return (char*) "Config File";
	case LMT_QT:                         return (char*) "Qt";
	}

	return (char*) 0;
}
