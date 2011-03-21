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



/* ****************************************************************************
*
* Forward declarationsaa of classes
*/
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
		WorkerAdded,
		WorkerDisconnected,
		WorkerRemoved,
		SelectToBeCalled,
		ProcessVectorReceived
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
		Unknown = 0,
		Worker,
		Controller,
		Spawner,
		Supervisor,
		Delilah,
		Killer,
		Setup,
		Listener,
		Sender,
		Temporal,
		CoreWorker,
		ThreadedReader,
		ThreadedSender,
		WebListener,
		WebWorker,
		Fd
	} Type;

private:
	char*                        alias;

public:
	std::string                  name;
	char*                        ip;
	struct sockaddr_in           sockin;

	void                         ipSet(const char* ip);
	void                         aliasSet(const char* alias);
	char*                        aliasGet(void);

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

	SendJobQueue*   jobQueueHead;

public:
	Endpoint(void);
	Endpoint(Type type, unsigned short port);
	Endpoint(Type type, const char* alias, int nada);
	Endpoint(Type type, std::string ipAndPort);
	Endpoint(Type type, std::string name, std::string ip, unsigned short port, int rFd, int wFd);
	~Endpoint();

	SendJob* jobPop(void);
	void     jobPush(SendJob*);
	void     jobInfo(int* messages, long long* dataLen);

	std::string str() { return name; }
};

}

#endif
