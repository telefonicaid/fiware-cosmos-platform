#ifndef ENDPOINT_H
#define ENDPOINT_H

/* ****************************************************************************
*
* FILE                      Endpoint.h
*
* DESCRIPTION				Class for endpoints
*
*/
#include <string>	        // std::string...

#include "workerStatus.h"	// CoreWorkerState, WorkerStatusData



namespace ss {



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
		Dead
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
		WebWorker
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
	int                          workerId;
	Type                         type;


	//
	// The following fields will form a union (depending on Type) in the near future
	//

	Message::WorkerStatusData*   status;           // Worker
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
	void            hostnameGet(void);

public:
	Endpoint(void) {};
	Endpoint(Type type, unsigned short port);
	Endpoint(Type type, char* alias);
	Endpoint(Type type, std::string ipAndPort);
	Endpoint(Type type, std::string name, std::string ip, unsigned short port, int rFd, int wFd);
	std::string str() { return name; }
};

}

#endif
