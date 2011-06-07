/* ****************************************************************************
*
* FILE                     Message.cpp - message definitions for all Samson IPC
*
*/
#include "samson/network/Message.h"            // Own interface



namespace samson
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
	case ProcessVector:                    return (char*) "ProcessVector";
	case WorkerVector:                     return (char*) "WorkerVector";
	case WorkerTask:                       return (char*) "WorkerTask";
	case WorkerTaskKill:                   return (char*) "WorkerTaskKill";
    case WorkerTaskRemove:                 return (char*) "WorkerTaskRemove";
	case WorkerTaskFinish:                 return (char*) "WorkerTaskFinish";
	case WorkerStatus:                     return (char*) "WorkerStatus";
	case Command:                          return (char*) "Command";
	case CommandResponse:		           return (char*) "CommandResponse";
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
	case Reset:                            return (char*) "Reset";
	case ProcessVectorGet:                 return (char*) "ProcessVectorGet";
	case Trace:                            return (char*) "Trace";
	case ProcessList:                      return (char*) "ProcessList";
    case PushBlock:                        return (char*) "PushBlock";
    case PushBlockResponse:                return (char*) "PushBlockResponse";
	case Id:                               return (char*) "Id";
	case Unknown:                          return (char*) "Unknown";
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
	case Msg:           return (char*) "Message";
	case Evt:           return (char*) "Event";
	case Ack:           return (char*) "Acknowledge";
	case Nak:           return (char*) "Neg Acknowledge";
	}

	return (char*) "Unknown";
}

}
}
