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
	case LmtEndpointListShow:            return (char*) "Show Endpoint List";
	case LmtStarterListShow:             return (char*) "Show Starter List (Supervisor only)";
	case LmtSpawnerListShow:             return (char*) "Show Spawner List (Supervisor only)";
	case LmtProcessListShow:             return (char*) "Show Process List (Supervisor only)";
	case LmtLogProviderListShow:         return (char*) "Show Log Provider List (LogServer only)";

	case LmtInit:                        return (char*) "Initialization, especially in main";
	case LmtConfigFile:                  return (char*) "Configuration file";
	case LmtDie:                         return (char*) "Die message sent/received";
	case LmtHello:                       return (char*) "Hello message interchange";
	case LmtTimeout:                     return (char*) "Timeout in Network select loop";
	case LmtMsgTreat:                    return (char*) "Message Treating";

	case LmtWorker:                      return (char*) "Worker";
	case LmtWorkers:                     return (char*) "Worker List";
	case LmtWorkerVector:                return (char*) "Worker Vector";

	case LmtEndpoint:                    return (char*) "Endpoint";
	case LmtEndpoints:                   return (char*) "Endpoints";
	case LmtEndpointUpdate:              return (char*) "Updating an Endpoint";
	case LmtTemporalEndpoint:            return (char*) "Temporal Endpoints";

	case LmtSpawnerList:                 return (char*) "Spawner List (Supervisor only)";
	case LmtProcessList:                 return (char*) "Process List (Supervisor only)";
	case LmtStarterList:                 return (char*) "Starter List (Supervisor only)";
	case LmtLogProviderList:             return (char*) "Log Provider List (Log Server only)";

	case LmtStarter:                     return (char*) "Starters (Supervisor only)";
	case LmtSpawner:                     return (char*) "Spawners (Supervisor only)";
	case LmtProcess:                     return (char*) "Processes (Supervisor only)";
	case LmtLogProvider:                 return (char*) "Log Provider";

	case LmtSpawnerConnect:              return (char*) "Connecting to Spawner";
	case LmtProcessConnect:              return (char*) "Connecting to Process";
	case LmtProcessStart:                return (char*) "Starting a Process";
	case LmtProcessKill:                 return (char*) "Killing a Process";

	case LmtEndpointLookup:              return (char*) "Endpoint Lookup";
	case LmtStarterLookup:               return (char*) "Looking up Starters (Supervisor only)";
	case LmtProcessLookup:               return (char*) "Looking up a Process (Supervisor only)";
	case LmtSpawnerLookup:               return (char*) "Looking up a Spawner (Supervisor only)";

	case LmtOpen:                        return (char*) "open system calls";
	case LmtConnect:                     return (char*) "socket connect system calls";
	case LmtAccept:                      return (char*) "Accept";
	case LmtRead:                        return (char*) "socket/fd read system calls";
	case LmtWrite:                       return (char*) "socket/fd write system calls";
	case LmtSelect:                      return (char*) "select system calls";
	case LmtReconnect:                   return (char*) "Reconnect";

	case LmtFds:                         return (char*) "File descriptors";
	case LmtNetworkReady:                return (char*) "Actions on Network module ready";
	case LmtSharedMemory:                return (char*) "Shared Memory";
	case LmtAlarm:                       return (char*) "At issuing an Alarm";
	case LmtSenderThread:                return (char*) "Network Sender Thread";
	case LmtThreadedMsgTreat:            return (char*) "Threaded Msg Treat function";
	case LmtMsgLoopBack:                 return (char*) "Network loops back messages for same machine";
	case LmtControllerConnect:           return (char*) "Connection to controller";

	case LmtNetworkInterface:            return (char*) "Network Interface";
	case LmtDelilah:                     return (char*) "Delilah";

	case LmtLogServer:                   return (char*) "Log Server";
	case LmtSpawn:                       return (char*) "Spawning processes (Spawner only)";

	case LmtProcessListTab:              return (char*) "Process List QT Tab";
	case LmtQtTimer:                     return (char*) "QT timers";

	case LmtCheck:                       return (char*) "Checkbox state (Supervisor only)";
	case LmtFile:                        return (char*) "File";
	case LmtTask:                        return (char*) "Task";
	case LmtJob:                         return (char*) "Job";
	case LmtSend:                        return (char*) "Send Callback";

	case LmtPopupMenu:                   return (char*) "Popup Menu";
	case LmtQueue:                       return (char*) "Queue";
	case LmtQueueConnection:             return (char*) "Queue Connection";
	case LmtQueueMgr:                    return (char*) "Queue Manager";
	case LmtSourceMgr:                   return (char*) "Source Manager";
	case LmtResultMgr:                   return (char*) "Result Manager";

	case LmtMouseEvent:                  return (char*) "Mouse Event";
	case LmtUser:                        return (char*) "User";
	}

	return (char*) 0;
}
