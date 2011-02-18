#ifndef ENDPOINT_H
#define ENDPOINT_H

/* ****************************************************************************
*
* FILE                      Endpoint.h
*
* DESCRIPTION               Class for endpoints
*
*/
#include <string>               // std::string ...
#include <pthread.h>            // pthread_t
#include <netinet/in.h>         // sockaddr_in

#include "Message.h"            // MessageCode, MessageType
#include "workerStatus.h"       // CoreWorkerState, WorkerStatusData



namespace ss {


class PacketSenderInterface;
class Endpoint;
class Packet;
class Network;

/* ****************************************************************************
*
* SendJob - 
*/
typedef struct SendJob
{
	Endpoint*             ep;
	Endpoint*             me;
	Message::MessageCode  msgCode;
	Message::MessageType  msgType;
	void*                 dataP;
	int                   dataLen;
	Packet*               packetP;
	Network*              network;
} SendJob;



/* ****************************************************************************
*
* SendJobQueue
*/
typedef struct SendJobQueue
{
	SendJob*             job;
	struct SendJobQueue* next;
} SendJobQueue;



class Endpoint
{
public:
	typedef enum UpdateReason
	{
		ControllerAdded,
		ControllerDisconnected,
		ControllerReconnected,
		ControllerRemoved,
		SupervisorAdded,
		EndpointRemoved,
		HelloReceived,
		NoLongerTemporal,
		WorkerAdded,
		WorkerDisconnected,
		WorkerRemoved,
		SelectToBeCalled,
		WorkerVectorReceived
	} UpdateReason;

	typedef enum State
	{
		Free,
		Me,
		FutureWorker,
		Listening,
		Unconnected,
		Connected,
		Closed,
		Disconnected,
		Reconnecting,
		Dead,
		Threaded
	} State;

	typedef enum Type
	{
		Unknown,
		Fd,
		Temporal,
		Listener,
		Controller,
		Worker,
		CoreWorker,
		ThreadedReader,
		ThreadedSender,
		Delilah,
		WebListener,
		WebWorker,
		Sender,
		Spawner,
		Supervisor
	} Type;

public:
	std::string                  name;
	std::string                  hostname;
	std::string                  ip;
	std::string                  alias;
	struct sockaddr_in           sockin;

	int                          rFd;
	int                          wFd;
	unsigned short               port;
	State                        state;
	int                          workers;
	Type                         type;
	bool                         helloReceived;

	// Message Statistics
	int                          msgsIn;
	int                          msgsOut;
	int                          msgsInErrors;
	int                          msgsOutErrors;
	long long                    bytesIn;
	long long                    bytesOut;
	int                          lastBps;

	// BPS Statistics
	int                          rMbps;
	int                          rAccMbps;
	int                          reads;
	int                          wMbps;
	int                          wAccMbps;
	int                          writes;

	int                          workerId;         // Worker
	Message::WorkerStatusData*   status;           // Worker
	PacketSenderInterface*       packetSender;     // Worker
	bool                         useSenderThread;  // Worker
	bool                         sender;           // Worker
	pthread_t                    senderTid;        // Worker
	int                          senderWriteFd;    // Worker
	int                          senderReadFd;     // Worker
	
	int                          coreNo;           // CoreWorker
	time_t                       startTime;        // CoreWorker
	int                          restarts;         // CoreWorker
	int                          jobsDone;         // CoreWorker
	Message::CoreWorkerState     coreWorkerState;  // CoreWorker

	const char*     stateName(void);
	const char*     typeName(void);
	const char*     typeName(Type type);
	const char*     nam();
	void            reset();
	void            init();
	void            hostnameGet(void);

	SendJobQueue*   jobQueueHead;

public:
	Endpoint(void);
	Endpoint(Type type, unsigned short port);
	Endpoint(Type type, char* alias);
	Endpoint(Type type, std::string ipAndPort);
	Endpoint(Type type, std::string name, std::string ip, unsigned short port, int rFd, int wFd);

	SendJob* jobPop(void);
	void     jobPush(SendJob*);

	std::string str() { return name; }
};

}

#endif
