#ifndef ENDPOINT_H
#define ENDPOINT_H

/* ****************************************************************************
*
* FILE                      Endpoint.h
*
* DESCRIPTION               Class for endpoints
*
*/
#include <string>            // std::string ...
#include <pthread.h>         // pthread_t

#include "workerStatus.h"    // CoreWorkerState, WorkerStatusData



namespace ss {


class PacketSenderInterface;

class Endpoint
{
public:
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
		Temporal,
		Listener,
		Controller,
		Worker,
		CoreWorker,
		Delilah,
		WebListener,
		WebWorker,
		Sender
	} Type;

public:
	std::string                  name;
	std::string                  hostname;
	std::string                  ip;
	std::string                  alias;
	int                          rFd;
	int                          wFd;
	unsigned short               port;
	State                        state;
	int                          workers;
	Type                         type;
	

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

public:
	Endpoint(void);
	Endpoint(Type type, unsigned short port);
	Endpoint(Type type, char* alias);
	Endpoint(Type type, std::string ipAndPort);
	Endpoint(Type type, std::string name, std::string ip, unsigned short port, int rFd, int wFd);

	std::string str() { return name; }
};

}

#endif
