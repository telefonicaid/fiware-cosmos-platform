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
char* traceLevelName(int level)
{
	switch (level)
	{
	case LMT_NWRUN:                      return (char*) "Nwrun";
	case LMT_SELECT:                     return (char*) "Select";
	case LMT_FDS:                        return (char*) "Fds";
	case LMT_ENDPOINT:                   return (char*) "Endpoint";
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
	case LMT_EP:                         return (char*) "Ep";
	case LMT_RESTART:                    return (char*) "Restart";
	case LMT_COREWORKER:                 return (char*) "Coreworker";
	case LMT_MSGTREAT:                   return (char*) "Msgtreat";
	case LMT_JOB:                        return (char*) "Job";
	case LMT_TIMEOUT:                    return (char*) "Timeout";
	case LMT_FORWARD:                    return (char*) "Forward";
	case LMT_SEND:                       return (char*) "Send";
	case LMT_PA:                         return (char*) "Pa";
	case LMT_FILE:                       return (char*) "File";
	case LMT_TASK:                       return (char*) "Task";
	case LMT_SAMSON_WORKER:              return (char*) "SamsonWorker";
	case LMT_WINIT:                      return (char*) "Winit";
	case LMT_LOG_PROVIDER_LIST:          return (char*) "LogProviderList";
	case LMT_SAMSON_DEMO:                return (char*) "SamsonDemo";
	case LMT_CONFIG_FILE:                return (char*) "ConfigFile";
	case LMT_CHECK:                      return (char*) "Check";
	case LMT_STARTER:                    return (char*) "Starter";
	case LMT_PROCESS_LIST:               return (char*) "ProcessList";
	case LMT_SPAWNER_LIST:               return (char*) "SpawnerList";
	case LMT_STARTER_LIST:               return (char*) "StarterList";
	case LMT_PROCESS_LIST_TAB:           return (char*) "ProcessListTab";
	case LMT_QT:                         return (char*) "Qt";
	}

	return (char*) 0;
}
