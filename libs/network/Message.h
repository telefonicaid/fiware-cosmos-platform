#ifndef SAMSON_MESSAGE_H
#define SAMSON_MESSAGE_H

/* ****************************************************************************
*
* FILE                     Message.h - message definitions for all Samson IPC
*
*/
#include "Endpoint.h"			// Endpoint::Type



namespace ss
{
namespace Message
{



/* ****************************************************************************
*
* CODE - Legible message codes and types
*/
#define CODE(c1, c2, c3, c4) ((c4 << 24) + (c3 << 16) + (c2 << 8) + c1)
	


/* ****************************************************************************
*
* MessageCode
*/
typedef enum MessageCode
{
	Alarm							= CODE('A', 'l', 'm', ' '),
	Hello							= CODE('H', 'e', 'l', ' '),
	WorkerVector					= CODE('W', 'V', 'c', ' '),
	WorkerStatus					= CODE('W', 'S', 't', ' '),
	WorkerTask						= CODE('W', 'T', 'a', ' '),
	WorkerDataExchange				= CODE('D', 'a', 't', ' '),
	WorkerDataExchangeClose			= CODE('D', 'e', 'c', ' '),
	WorkerTaskFinish				= CODE('D', 'T', 'f', ' '),
	WorkerTaskConfirmation			= CODE('W', 'T', 'C', ' '),
	Command							= CODE('C', 'o', 'm', ' '),
	CommandResponse					= CODE('C', 'R', 'e', ' '),
	Help							= CODE('H', 'e', 'p', ' '),
	HelpResponse					= CODE('H', 'R', 'e', ' '),
	StatusRequest					= CODE('S', 'r', 'q', ' '),
	StatusResponse					= CODE('S', 'r', 's', ' '),
	LoadData						= CODE('L', 'D', 'a', ' '),
	LoadDataResponse				= CODE('L', 'D', 'R', ' '),
	LoadDataConfirmation			= CODE('L', 'C', 'o', ' '),
	LoadDataConfirmationResponse	= CODE('L', 'C', 'R', ' '),
	Die								= CODE('D', 'i', 'e', ' ')
} MessageCode;



/* ****************************************************************************
*
* MessageType
*/
typedef enum MessageType
{
	Msg = CODE('M', 'S', 'G', ' '),
	Evt = CODE('E', 'V', 'T', ' '),
	Ack = CODE('A', 'C', 'K', ' '),
	Nak = CODE('N', 'A', 'K', ' ')
} MessageType;



/* ****************************************************************************
*
* Header - 
*/
typedef struct Header
{
	MessageCode    code;
	MessageType    type;
	unsigned int   dataLen;
	unsigned int   gbufLen;
	unsigned int   kvDataLen;
} Header;



/* ****************************************************************************
*
* HelloData
*/
typedef struct HelloData
{
	char                name[32];
	char                ip[32];
	char                alias[32];
	ss::Endpoint::Type  type;
	int                 workers;
	int                 port;
	int                 coreNo;
	int                 workerId;
} HelloData;



/* ****************************************************************************
*
* Worker
*/
typedef struct Worker
{
	char             name[32];
	char             alias[32];
	char             ip[32];
	int              port;
	Endpoint::State  state;
} Worker;



/* ****************************************************************************
*
* messageCode - 
*/
extern char* messageCode(MessageCode code);



/* ****************************************************************************
*
* messageType - 
*/
extern char* messageType(MessageType type);

}
}

#endif
