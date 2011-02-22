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
	Help							= CODE('H', 'e', 'p', ' '),
	HelpResponse					= CODE('H', 'R', 'e', ' '),
	StatusRequest					= CODE('S', 'r', 'q', ' '),
	StatusResponse					= CODE('S', 'r', 's', ' '),
	DownloadData					= CODE('D', 'L', 'a', ' '),
	DownloadDataResponse			= CODE('D', 'L', 'R', ' '),
	UploadData						= CODE('L', 'D', 'a', ' '),
	UploadDataResponse				= CODE('L', 'D', 'R', ' '),
	UploadDataConfirmation			= CODE('L', 'C', 'o', ' '),
	UploadDataConfirmationResponse	= CODE('L', 'C', 'R', ' '),
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
* Worker
*/
typedef struct Worker
{
	char             name[32];
	char             alias[32];
	char             ip[32];
	int              port;
	int              state;
	bool             verbose;
	bool             debug;
	bool             reads;
	bool             writes;
	bool             toDo;
	char             traceV[256];
} Worker;



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
* WorkerVectorData - 
*/
typedef struct WorkerVectorData
{
	int      workers;
	Worker   workerV[0];
} WorkerVectorData;



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
