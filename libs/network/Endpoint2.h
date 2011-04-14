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
#include <netinet/in.h>         // sockaddr_in

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



/* ****************************************************************************
*
* Endpoint2 - 
*/
class Endpoint2
{
public:
	typedef enum State
	{
		NeverConnected,
		Listening,
		Connected,
		ScheduledForRemoval,
		Reconnecting          // Disconnected ?
	} State;

	typedef enum Type
	{
		Anonymous    = 0,
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
	Endpoint2(Type _type, const char* _name, const char* _alias, Host* _host, unsigned short _port = 0, int _rFd = -1, int _wFd = -1);
	~Endpoint2();

private:
	Host*                host;
	Type                 type;
	char*                name;
	char*                alias;
	unsigned short       port;
	int                  rFd;
	int                  wFd;
	State                state;
	struct sockaddr_in   sockin;
	int                  workerId;         // Worker
	bool                 useSenderThread;  // Worker/Delilah
	JobQueue*            jobQ;

public:
	static const char*   typeName(Type type);
	const char*          typeName();
	Type                 typeGet();
	void                 typeSet(Type type);

	Host*                hostGet(void);
	const char*          hostname(void);

	void                 portSet(unsigned short _port);
	unsigned short       portGet(void);

	void                 nameSet(const char* name);
	const char*          nameGet(void);

	void                 aliasSet(const char* alias);
	const char*          aliasGet(void);

	void                 helloDataAdd(EndpointManager* epMgr, Type _type, const char* _name, const char* _alias);

	Endpoint2*           connect(bool addToEpVec);   // the fd from connect is added as an Anonymous-typed endpoint
	Endpoint2*           accept(bool addToEpVec);
	int                  msgAwait(int secs, int usecs);
	int                  receive(Endpoint2* from, Message::Header* headerP, void** dataPP, int* dataLenP, ss::Packet* packetP);
	int                  send(Endpoint2* to, Message::MessageType type, Message::MessageCode code, void* data = NULL, int dataLen = 0, Packet* packetP = NULL);
	int                  helloSend(Endpoint2* self, Message::MessageType type);   // send Hello Msg/Ack/Nak to endpoint

	int                  msgTreat(void);
	virtual int          msgTreat2() { /* To be implemented */ LM_X(1, ("NOT IMPLEMENTED")); return 1; };

private:
	int                  listenerPrepare(void);
	                                                                       // Or, the accepted fd is added as an Anonymous endpoint?
};

}

#endif
