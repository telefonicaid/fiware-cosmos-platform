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
	Alarm                  = CODE('A', 'l', 'm', ' '),
	Hello                  = CODE('H', 'e', 'l', ' '),
	WorkerVector           = CODE('W', 'V', 'c', ' '),
	WorkerStatus           = CODE('W', 'S', 't', ' '),
	WorkerTask             = CODE('W', 'T', 'a', ' '),
	Command                = CODE('C', 'o', 'm', ' '),
	CommandResponse        = CODE('C', 'R', 'e', ' '),
	WorkerTaskConfirmation = CODE('W', 'T', 'C', ' '),
	Job                    = CODE('J', 'o', 'b', ' '),
	JobDone                = CODE('D', 'o', 'n', ' ')
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
* JobData - 
*/
typedef struct JobData
{
	int  coreNo;
	char description[64];
} JobData;



/* ****************************************************************************
*
* HelloData
*/
typedef struct HelloData
{
	char                name[32];
	char                ip[32];
	ss::Endpoint::Type  type;
	int                 workers;
	int                 port;
	int                 coreNo;
} HelloData;



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
