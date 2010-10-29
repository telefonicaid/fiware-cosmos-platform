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
	case WorkerVector:             return (char*) "WorkerVector";
	case WorkerStatus:             return (char*) "WorkerStatus";
	case WorkerTask:               return (char*) "WorkerTask";
	case Command:                  return (char*) "Command";
	case CommandResponse:          return (char*) "CommandResponse";
	case WorkerTaskConfirmation:   return (char*) "WorkerTaskConfirmation";
	case Job:                      return (char*) "Job";
	case JobDone:                  return (char*) "JobDone";
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
