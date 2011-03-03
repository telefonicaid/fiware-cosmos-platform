#ifndef SAMSON_MESSAGE_H
#define SAMSON_MESSAGE_H

/* ****************************************************************************
*
* FILE                     Message.h - message definitions for all Samson IPC
*
*/
#include "Process.h"            // Process



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
	ProcessVector					= CODE('P', 'V', 'c', ' '),
	WorkerVector					= CODE('W', 'V', 'c', ' '),
	WorkerStatus					= CODE('W', 'S', 't', ' '),
	WorkerTask						= CODE('W', 'T', 'a', ' '),
	WorkerTaskKill					= CODE('W', 'T', 'k', ' '),
	WorkerDataExchange				= CODE('D', 'a', 't', ' '),
	WorkerDataExchangeClose			= CODE('D', 'e', 'c', ' '),
	WorkerTaskFinish				= CODE('D', 'T', 'f', ' '),
	WorkerTaskConfirmation			= CODE('W', 'T', 'C', ' '),
	Command							= CODE('C', 'o', 'm', ' '),
	CommandResponse					= CODE('C', 'R', 'e', ' '),
	StatusRequest					= CODE('S', 'r', 'q', ' '),
	StatusResponse					= CODE('S', 'r', 's', ' '),
	UploadDataInit					= CODE('U', 'D', 'a', ' '),
	UploadDataInitResponse			= CODE('U', 'D', 'b', ' '),
	UploadDataFile					= CODE('U', 'D', 'c', ' '),
	UploadDataFileResponse			= CODE('U', 'D', 'd', ' '),
	UploadDataFinish				= CODE('U', 'D', 'e', ' '),
	UploadDataFinishResponse		= CODE('U', 'D', 'f', ' '),
	DownloadDataInit         		= CODE('D', 'D', 'a', ' '),
	DownloadDataInitResponse		= CODE('D', 'D', 'b', ' '),
	DownloadDataFile				= CODE('D', 'D', 'c', ' '),
	DownloadDataFileResponse	    = CODE('D', 'D', 'd', ' '),
	DownloadDataFinish			    = CODE('D', 'D', 'e', ' '),
	DownloadDataFinishResponse	    = CODE('D', 'D', 'f', ' '),
	Die								= CODE('D', 'i', 'e', ' '),
	IDie							= CODE('I', 'D', 'i', ' '),
	WorkerSpawn                     = CODE('W', 'S', 'p', ' '),
	ControllerSpawn                 = CODE('C', 'S', 'p', ' '),
	ProcessSpawn                    = CODE('S', 'p', 'w', ' '),
	ThroughputTest                  = CODE('T', 'p', 'u', ' '),
	LogLine                         = CODE('L', 'o', 'g', ' '),
	ConfigGet                       = CODE('C', 'f', 'G', ' '),
	ConfigSet                       = CODE('C', 'f', 'S', ' '),
	LogSendingOn                    = CODE('L', 'O', 'n', ' '),
	LogSendingOff                   = CODE('L', 'O', 'f', ' '),
	ConfigChange                    = CODE('C', 'f', 'C', ' '),
	WorkerConfigGet                 = CODE('W', 'C', 'G', ' '),
	EntireLogFile                   = CODE('E', 'L', 'F', ' '),
	EntireOldLogFile                = CODE('O', 'L', 'F', ' ')
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
	unsigned int   magic;
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
	int                 type;
	int                 workers;
	int                 port;
	int                 coreNo;
	int                 workerId;
} HelloData;



/* ****************************************************************************
*
* ConfigData
*/
typedef struct ConfigData
{
	char   name[32];
	char   alias[32];
	char   host[32];
	bool   verbose;
	bool   debug;
	bool   reads;
	bool   writes;
	bool   toDo;
	char   traceLevels[256];
} ConfigData;



/* ****************************************************************************
*
* SpawnData - 
*/
typedef struct SpawnData
{
	char            name[32];
	int             argCount;
	char            args[256];
} SpawnData;



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
