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



/* ****************************************************************************
*
* Endpoint2 - 
*/
class Endpoint2
{
   friend class EndpointManager;
   friend class ListenerEndpoint;

public:
	typedef enum Status
	{
		OK,
		NotImplemented,
		ConnectionClosed,
		ReadError,
		WriteError,
		Timeout,

		NullAlias,
		BadAlias,
		BadHost,
		Duplicated,
		AcceptError,
		NotListener,
		SelectError,
		Error,
		SocketError,
		BindError,
		ListenError
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
		Worker       = 1, /* Must be the same value as libs/common/Process.h:PtWorker      */
		Controller   = 2, /* Must be the same value as libs/common/Process.h:PtController  */
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

public:
	static const char*   typeName(Type type);
	const char*          typeName();
	Type                 typeGet();
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



	Endpoint2*           connect(void);
	Status               msgAwait(int secs, int usecs);

	Status               partRead(void* vbuf, long bufLen, long* bufLenP = NULL);
	Status               receive(Message::Header* headerP, void** dataPP, int* dataLenP, Packet* packetP);

	Status               okToSend(void);
	Status               partSend(void* dataP, int dataLen, const char* what);
	Status               send(Message::MessageType type, Message::MessageCode code, void* data = NULL, int dataLen = 0, Packet* packetP = NULL);

	void                 run(void);
	Status               msgTreat(void);

	virtual Status       msgTreat2(Message::Header* headerP, void* dataP, int dataLen, Packet* packetP)
	{
	   LM_X(1, ("NOT IMPLEMENTED")); return NotImplemented;
	};

	virtual Status       msgTreat2(void)
	{
	   LM_X(1, ("NOT IMPLEMENTED")); return NotImplemented;
	};

private:
	Status               listenerPrepare(void);
};

}

#endif
