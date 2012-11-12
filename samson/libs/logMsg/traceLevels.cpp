/*
 * Telefónica Digital - Product Development and Innovation
 *
 * THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND,
 * EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
 *
 * Copyright (c) Telefónica Investigación y Desarrollo S.A.U.
 * All rights reserved.
 */
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
	case LmtProcessVector:               return (char*) "Process Vector";

	case LmtInit:                        return (char*) "Initialization, especially in main";
	case LmtConfigFile:                  return (char*) "Configuration file";
	case LmtDie:                         return (char*) "Die message sent/received";
	case LmtHello:                       return (char*) "Hello message interchange";
	case LmtTimeout:                     return (char*) "Timeout in Network select loop";
	case LmtMsgTreat:                    return (char*) "Message Treating";
	case LmtHost:                        return (char*) "Host";

	case LmtWorker:                      return (char*) "Worker";
	case LmtWorkers:                     return (char*) "Worker List";

	case LmtEndpoint:                    return (char*) "Endpoint";
	case LmtEndpoints:                   return (char*) "Endpoints";
	case LmtEndpointUpdate:              return (char*) "Updating an Endpoint";
	case LmtEndpointSlots:               return (char*) "Endpoint Slots";
	case LmtEndpointAdd:                 return (char*) "Endpoint Add";
	case LmtTemporalEndpoint:            return (char*) "Temporal Endpoints";
	case LmtIdIx:                        return (char*) "Endpoint Id/Ix";
	case LmtUnhelloed:                   return (char*) "Unhelloed Endpoint";

	case LmtSpawnerList:                 return (char*) "Spawner List (Supervisor only)";
	case LmtProcessList:                 return (char*) "Process List (Supervisor only)";
	case LmtStarterList:                 return (char*) "Starter List (Supervisor only)";

	case LmtStarter:                     return (char*) "Starters (Supervisor only)";
	case LmtSpawner:                     return (char*) "Spawners (Supervisor only)";
	case LmtProcess:                     return (char*) "Processes (Supervisor only)";
	case LmtSetup:                       return (char*) "Setup";

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
	case LmtAlarm:                       return (char*) "At issuing an Alarm";
	case LmtSenderThread:                return (char*) "Network Sender Thread";
	case LmtThreadedMsgTreat:            return (char*) "Threaded Msg Treat function";
	case LmtMsgLoopBack:                 return (char*) "Network loops back messages for same machine";
	case LmtControllerConnect:           return (char*) "Connection to controller";
	case LmtPacketReceive:               return (char*) "Packet Receive";

	case LmtNetworkInterface:            return (char*) "Network_Interface";
	case LmtDelilah:                     return (char*) "Delilah";

	case LmtLogServer:                   return (char*) "Log Server";
	case LmtSpawn:                       return (char*) "Spawning processes (Spawner only)";

	case LmtProcessListTab:              return (char*) "Process List QT Tab";
	case LmtQtTimer:                     return (char*) "QT timers";

	case LmtCheck:                       return (char*) "Check";
	case LmtFile:                        return (char*) "File";
	case LmtTask:                        return (char*) "Task";
	case LmtJob:                         return (char*) "Job";
	case LmtSend:                        return (char*) "Send Callback";
	case LmtReceive:                     return (char*) "Msg Receive";

	case LmtPopupMenu:                   return (char*) "Popup Menu";
	case LmtQueue:                       return (char*) "Queue";
	case LmtResult:                      return (char*) "Result";
	case LmtSource:                      return (char*) "Source";
	case LmtConnection:                  return (char*) "Connection";
	case LmtQueueMgr:                    return (char*) "Queue_Manager";
	case LmtSourceMgr:                   return (char*) "Source_Manager";
	case LmtResultMgr:                   return (char*) "Result_Manager";

	case LmtMouseEvent:                  return (char*) "Mouse_Event";
	case LmtUser:                        return (char*) "User";

	case LmtSceneItemChain:              return (char*) "Scene_Item_Chain";
	case LmtMove:                        return (char*) "Move";

    case LmtWait:                        return (char*) "Wait";
    case LmtReset:                       return (char*) "Reset";

    case LmtExcesiveTime:                return (char*) "ExcesiveTime";

    case LmtEngine:                      return (char*) "Engine";
    case LmtEngineNotification:          return (char*) "Engine_Notification";
    case LmtEngineTime:                  return (char*) "EngineTime";

    case LmtModuleManager:				 return (char*) "ModuleManager";
	case LmtDisk:                        return (char*) "Disk_library";
	case LmtMemory:						 return (char*) "Memory_library";
	case LmtProcessManager:              return (char*) "Process_Manager";
	case LmtBlockManager:                return (char*) "Block_Manager";

	case LmtModuleManager:               return (char*) "Module_Manager";
	case LmtOperations:                  return (char*) "Operations";

	case LmtIsolated:                    return (char*) "Isolated_process";

	case LmtUser01:                      return (char*) "User_01";
	case LmtUser02:                      return (char*) "User_02";
	case LmtUser03:                      return (char*) "User_03";
	case LmtUser04:                      return (char*) "User_04";
	case LmtUser05:                      return (char*) "User_05";
	case LmtUser06:                      return (char*) "User_06";
	}

	return (char*) 0;
}
