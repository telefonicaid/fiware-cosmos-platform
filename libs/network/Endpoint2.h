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
#include <stdio.h>              // NULL
#include <netinet/in.h>         // sockaddr_in

#include "logMsg.h"             // LM_X

#include "Message.h"            // Message::Code, Message::Type
#include "Host.h"               // Host
#include "Process.h"            // PtWorker, PtController
#include "JobQueue.h"           // JobQueue



namespace ss
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



/* ****************************************************************************
*
* Endpoint2 - 
*/
class Endpoint2
{
	friend class EndpointManager;
	friend class ListenerEndpoint;
	friend class UnhelloedEndpoint;
	friend class WebListenerEndpoint;
	friend class WebWorkerEndpoint;

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

	typedef enum Status
	{
		OK,
		NotImplemented,

		NullAlias,
		BadAlias,
		NullHost,
		BadHost,
		NullPort,
		Duplicated,
		KillError,
		NotHello,
		NotAck,
		NotMsg,

		Error,
		ConnectError,
		AcceptError,
		NotListener,
		SelectError,
		SocketError,
		GetHostByNameError,
		BindError,
		ListenError,
		ReadError,
		WriteError,
		Timeout,
		ConnectionClosed
	} Status;

	typedef enum State
	{
		Unused,
		Ready,
		Disconnected,
		ScheduledForRemoval
	} State;

	typedef enum Type  // Move this enum to common/Process, called ProcessType
	{
		Unhelloed    = 0,
		Worker       = 1, // Must be the same value as libs/common/Process.h:PtWorker
		Controller   = 2, // Must be the same value as libs/common/Process.h:PtController
		Spawner,
		Supervisor,
		Delilah,
		Killer,
		Setup,
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
		const char*      _name,
		const char*      _alias,
		Host*            _host,
		unsigned short   _port  = 0,
		int              _rFd   = -1,
		int              _wFd   = -1
	);

	~Endpoint2();
	const char*          status(Status s);

private:
	EndpointManager*     epMgr;

	Type                 type;
	int                  id;
	Host*                host;
	char*                name;
	char*                alias;
	unsigned short       port;
	int                  rFd;
	int                  wFd;
	State                state;
	struct sockaddr_in   sockin;
	bool                 useSenderThread;  // Worker/Delilah
	JobQueue*            jobQ;


	// Statistics
	int                  msgsIn;
	int                  msgsOut;
	int                  msgsInErrors;
	int                  msgsOutErrors;

public:
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

	void                 nameSet(const char* name);
	const char*          nameGet(void);

	void                 aliasSet(const char* alias);
	const char*          aliasGet(void);

	State                stateGet(void);
	void                 stateSet(State _state);

	int                  rFdGet(void);
	int                  wFdGet(void);



	Status               connect(void);
	Status               msgAwait(int secs, int usecs);

	Status               partRead(void* vbuf, long bufLen, long* bufLenP = NULL);
	Status               receive(Message::Header* headerP, void** dataPP, int* dataLenP, Packet* packetP);

	Status               okToSend(void);
	Status               partSend(void* dataP, int dataLen, const char* what);
	Status               send(Message::MessageType typ, Message::MessageCode code, void* data = NULL, int dataLen = 0, Packet* packetP = NULL);
	Status               ack(Message::MessageCode code, void* data = NULL, int dataLen = 0);
	size_t               send(PacketSenderInterface* psi, Message::MessageCode code, Packet* packetP);
	virtual void         run(void);
	Status               msgTreat(void);

	virtual Status       msgTreat2(Message::Header* headerP, void* dataP, int dataLen, Packet* packetP)
	{
	   LM_X(1, ("NOT IMPLEMENTED")); return NotImplemented;
	};

	virtual Status       msgTreat2(void)
	{
	   LM_X(1, ("NOT IMPLEMENTED")); return NotImplemented;
	};

	Status               hello(int secs, int usecs = 0);     // send hello and await ack, with timeout
	Status               die(int secs, int usecs = 0);       // send 'die' to endpoint, and await death, with timeout

private:
	Status               listenerPrepare(void);
};

}

#endif
