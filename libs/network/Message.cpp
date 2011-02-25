/* ****************************************************************************
*
* FILE                     Message.cpp - message definitions for all Samson IPC
*
*/
#include "Message.h"            // Own interface



namespace ss
{
namespace Message
{


/* ****************************************************************************
*
* messageCode - 
*/
char* messageCode(MessageCode code)
{
	switch (code)
	{
		case Alarm:                            return (char*) "Alarm";
		case Hello:                            return (char*) "Hello";
		case WorkerTask:                       return (char*) "WorkerTask";
		case WorkerTaskKill:                   return (char*) "WorkerTaskKill";
		case WorkerTaskFinish:                 return (char*) "WorkerTaskFinish";
		case WorkerVector:                     return (char*) "WorkerVector";
		case WorkerStatus:                     return (char*) "WorkerStatus";
		case Command:                          return (char*) "Command";
		case CommandResponse:		           return (char*) "CommandResponse";
		case Help:                             return (char*) "Help";
		case HelpResponse:		               return (char*) "HelpResponse";
		case StatusRequest:                    return (char*) "StatusRequest";
		case StatusResponse:		           return (char*) "StatusResponse";
		case UploadDataInit:                   return (char*) "UploadDataInit";
		case UploadDataInitResponse:           return (char*) "UploadDataInitResponse";
		case UploadDataFile:                   return (char*) "UploadDataFile";
		case UploadDataFileResponse:           return (char*) "UploadDataFileResponse";
		case UploadDataFinish:                 return (char*) "UploadDataFinish";
		case UploadDataFinishResponse:         return (char*) "UploadDataFinishResponse";
		case DownloadDataInit:                 return (char*) "DownloadDataInit";
		case DownloadDataInitResponse:         return (char*) "DownloadDataInitResponse";
		case DownloadDataFile:                 return (char*) "DownloadDataFile";
		case DownloadDataFileResponse:         return (char*) "DownloadDataFileResponse";
		case DownloadDataFinish:               return (char*) "DownloadDataFinish";
		case DownloadDataFinishResponse:       return (char*) "DownloadDataFinishResponse";
		case WorkerTaskConfirmation:           return (char*) "WorkerTaskConfirmation";
		case WorkerDataExchange:	           return (char*) "WorkerDataExchange";
		case WorkerDataExchangeClose:          return (char*) "WorkerDataExchangeClose";
		case Die:                              return (char*) "Die";
		case IDie:                             return (char*) "IDie";
		case WorkerSpawn:                      return (char*) "WorkerSpawn";
		case ControllerSpawn:                  return (char*) "ControllerSpawn";
		case ProcessSpawn:                     return (char*) "ProcessSpawn";
		case ThroughputTest:                   return (char*) "ThroughputTest";
		case LogLine:                          return (char*) "LogLine";
		case ConfigGet:                        return (char*) "ConfigGet";
		case ConfigSet:                        return (char*) "ConfigSet";
		case LogSendingOn:                     return (char*) "LogSendingOn";
		case LogSendingOff:                    return (char*) "LogSendingOff";
		case ConfigChange:                     return (char*) "ConfigChange";
		case WorkerConfigGet:                  return (char*) "WorkerConfigGet";
	case EntireLogFile:                    return (char*) "EntireLogFile";
	case EntireOldLogFile:                 return (char*) "EntireOldLogFile";
	}

	return (char*) "Unknown";
}



/* ****************************************************************************
*
* messageType - 
*/
char* messageType(MessageType type)
{
	switch (type)
	{
	case Msg:           return (char*) "Msg";
	case Evt:           return (char*) "Evt";
	case Ack:           return (char*) "Ack";
	case Nak:           return (char*) "Nak";
	}

	return (char*) "Unknown";
}

}
}
