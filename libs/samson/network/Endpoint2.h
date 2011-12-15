#ifndef ENDPOINT2_H
#define ENDPOINT2_H

/* ****************************************************************************
*
* FILE                     Endpoint2.h
*
* DESCRIPTION              Class for endpoints
*
* AUTHOR                   Ken Zangelin
*
* CREATION DATE            Apr 12 2011
*
*/
#include <stdio.h>                  // NULL
#include <netinet/in.h>             // sockaddr_in

#include "logMsg/logMsg.h"          // LM_X

#include "au/Token.h"
#include "au/list.h"
#include "samson/common/status.h"
#include "samson/common/Process.h"  // PtWorker, PtController
#include "Message.h"                // Message::Code, Message::Type
#include "Host.h"                   // Host
#include "NetworkInterface.h"       // PacketSenderInterface



namespace samson
{



/* ****************************************************************************
*
* Forward declarations of classes
*/
class Endpoint2;
class Packet;
class EndpointManager;
class ListenerEndpoint;
class UnhelloedEndpoint;
class WebListenerEndpoint;
class WebWorkerEndpoint;
class SpawnerEndpoint;



/* ****************************************************************************
*
* Endpoint2 - 
*/
class Endpoint2
{
	friend class Network2;
	friend class EndpointManager;

	friend class ListenerEndpoint;
	friend class UnhelloedEndpoint;
	friend class WebListenerEndpoint;
	friend class WebWorkerEndpoint;
	friend class SpawnerEndpoint;
	friend class WorkerEndpoint;
	friend class ControllerEndpoint;
	friend class DelilahEndpoint;
	friend class StarterEndpoint;

public:
	typedef enum State
	{
		Unused,
		Loopback,
		Connected,
		Ready,        // Hello interchanged
		Disconnected,
		ScheduledForRemoval
	} State;

	typedef enum Type  // Move this enum to common/Process, call it ProcessType
	{
		Unhelloed    = 0,
		Worker       = 1, // Must be the same value as libs/common/Process.h:PtWorker
		Controller   = 2, // Must be the same value as libs/common/Process.h:PtController
		Spawner,
		Delilah,
		Starter,
		Supervisor,
		Killer,
		Listener,
		Sender,
		CoreWorker,
		ThreadReader,
		ThreadSender,
		WebListener,
		WebWorker
	} Type;


public:
	Endpoint2
	(
		EndpointManager* _epMgr,
		Type             _type,
		int              _id,
		Host*            _host,
		unsigned short   _port  = 0,
		int              _rFd   = -1,
		int              _wFd   = -1
	);

	~Endpoint2();

	const char*          name(void);
	void                 nameSet(Type _type, int id, Host* host);
	const char*          stateName(void);

	au::Token            jobQueueSem;
	au::list<Packet>     jobQueue;

    bool thread_writer_running;     // Flag to indicate that there is a thread using this endpoint writing data
    bool thread_reader_running;     // Flag to indicate that there is a thread using this endpoint reading data
    
private:
	EndpointManager*     epMgr;
	char*                nameidhost;
	int                  idInEndpointVector;
	Type                 type;
	int                  id;
	Host*                host;
	unsigned short       port;
	int                  rFd;
	int                  wFd;
	State                state;
	struct sockaddr_in   sockin;
	bool                 threaded;  // Worker/Delilah

public:
	pthread_t            readerId;
	pthread_t            writerId;

	// Statistics
	int                  msgsIn;
	int                  msgsOut;
	size_t               bytesIn;
	size_t               bytesOut;
	int                  msgsInErrors;
	int                  msgsOutErrors;

	EndpointManager*     epMgrGet();

	static const char*   typeName(Type type);
	const char*          typeName(void);
	Type                 typeGet(void);
	void                 typeSet(Type type);

	void                 idSet(int _id);
	int                  idGet(void);

	Host*                hostGet(void);
	Host*                hostSet(Host* hostP);
	const char*          hostname(void);

	void                 portSet(unsigned short _port);
	unsigned short       portGet(void);

	State                stateGet(void);
	void                 stateSet(State _state);

	int                  rFdGet(void);
	int                  wFdGet(void);

	bool                 isThreaded();

	Status               connect(void);
	const char*          statusString(char* buf, int bufLen, int ix, bool carriageReturn = true);
	Status               msgAwait(int secs, int usecs, const char* what);

	Status               receive(Message::Header* headerP, void** dataPP, long* dataLenP, Packet* packetP);

	Status               okToSend(void);
	void                 send(Packet* packetP);
	void                 ack(Message::MessageCode code, void* data = NULL, int dataLen = 0);

	virtual void         run(void);
	Status               msgTreat(void);

	virtual Status       msgTreat2(Packet* packetP)
	{
		LM_X(1, ("msgTreat2 NOT IMPLEMENTED for %s", typeName())); return NotImplemented;
	};

	virtual Status       msgTreat2(void)
	{
		LM_X(1, ("msgTreat2(void) NOT IMPLEMENTED for %s", typeName())); return NotImplemented;
	};

	Status               die(int secs, int usecs = 0);       // send 'die' to endpoint, and await death, with timeout

	Status               helloDataSet(Type _type, int _id);
	void                 helloSend(Message::MessageType type);   // send Hello Msg/Ack/Nak to endpoint

	Status               realsend(Message::MessageType typ, Message::MessageCode code, void* data = NULL, int dataLen = 0, Packet* packetP = NULL);

    int                  getIdInEndpointVector();   
    
    size_t               getOutputBufferSize();
    
private:
	Status               partRead(void* vbuf, long bufLen, long* bufLenP, const char* what);
	Status               partWrite(void* dataP, int dataLen, const char* what);
	void                 close(void);
};

}

#endif
