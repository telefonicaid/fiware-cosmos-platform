#ifndef SAMSON_MESSAGE_H
#define SAMSON_MESSAGE_H

/* ****************************************************************************
*
* FILE                     Message.h - message definitions for all Samson IPC
*
*/
#include "Alarm.h"              // 
#include "Endpoint.h"			// Endpoint::Type



namespace ss
{
namespace Message
{


/* ****************************************************************************
*
* MessageCode
*/
typedef enum MessageCode
{
	Hello                  = ('H' << 24) + ('E' << 16) + ('L' << 8) + 'O',
	WorkerVector           = 'V',
	WorkerStatus           = 'S',
	WorkerTask             = 'T',
	Command                = 'C',
	CommandResponse        = 'R',
	WorkerTaskConfirmation = 'c',
	Job                    = 'J',
	JobDone                = 'j'
} MessageCode;



/* ****************************************************************************
*
* MessageType
*/
typedef enum MessageType
{
   Msg = 'M',
   Evt = 'E',
   Ack = 'A',
   Nak = 'N'
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
