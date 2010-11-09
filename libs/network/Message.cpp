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
	case Alarm:                    return (char*) "Alarm";
	case Hello:                    return (char*) "Hello";
	case WorkerTask:               return (char*) "WorkerTask";
	case WorkerVector:             return (char*) "WorkerVector";
	case WorkerStatus:             return (char*) "WorkerStatus";
	case Command:                  return (char*) "Command";
	case CommandResponse:		   return (char*) "CommandResponse";
	case Help:                     return (char*) "Help";
	case HelpResponse:		       return (char*) "HelpResponse";
	case LoadData:								return (char*) "LoadData";
	case LoadDataResponse:						return (char*) "LoadDataResponse";
	case LoadDataConfirmation:                  return (char*) "LoadDataConfirmation";
	case LoadDataConfirmationResponse:          return (char*) "LoadDataConfirmationResponse";
	case WorkerTaskConfirmation:   return (char*) "WorkerTaskConfirmation";
	case WorkerDataExchange:	   return (char*) "WorkerDataExchange";
	case Die:                      return (char*) "Die";

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
