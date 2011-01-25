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
	case WorkerTaskFinish:                 return (char*) "WorkerTaskFinish";
	case WorkerVector:                     return (char*) "WorkerVector";
	case WorkerStatus:                     return (char*) "WorkerStatus";
	case Command:                          return (char*) "Command";
	case CommandResponse:		           return (char*) "CommandResponse";
	case Help:                             return (char*) "Help";
	case HelpResponse:		               return (char*) "HelpResponse";
	case StatusRequest:                    return (char*) "StatusRequest";
	case StatusResponse:		           return (char*) "StatusResponse";
	case DownloadData:                     return (char*) "DownloadData";
	case DownloadDataResponse:             return (char*) "DownloadDataResponse";
	case UploadData:                       return (char*) "LoadData";
	case UploadDataResponse:               return (char*) "LoadDataResponse";
	case UploadDataConfirmation:           return (char*) "LoadDataConfirmation";
	case UploadDataConfirmationResponse:   return (char*) "LoadDataConfirmationResponse";
	case WorkerTaskConfirmation:           return (char*) "WorkerTaskConfirmation";
	case WorkerDataExchange:	           return (char*) "WorkerDataExchange";
	case WorkerDataExchangeClose:          return (char*) "WorkerDataExchangeClose";
	case Die:                              return (char*) "Die";
	case IDie:                             return (char*) "IDie";
	case WorkerSpawn:                      return (char*) "WorkerSpawn";
	case ControllerSpawn:                  return (char*) "ControllerSpawn";
	case ThroughputTest:                   return (char*) "ThroughputTest";
	case LogLine:                          return (char*) "LogLine";
	case ConfigGet:                        return (char*) "ConfigGet";
	case ConfigSet:                        return (char*) "ConfigSet";
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
