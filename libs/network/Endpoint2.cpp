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
#include <sys/types.h>          // types needed by socket include files
#include <stdlib.h>             // free
#include <sys/socket.h>         // socket, bind, listen
#include <sys/un.h>             // sockaddr_un
#include <netinet/in.h>         // struct sockaddr_in
#include <netdb.h>              // gethostbyname
#include <arpa/inet.h>          // inet_ntoa
#include <netinet/tcp.h>        // TCP_NODELAY
#include <unistd.h>             // close
#include <fcntl.h>              // fcntl, F_SETFD

#include "logMsg.h"             // LM_*
#include "traceLevels.h"        // Trace Levels

#include "ports.h"              // WORKER_PORT, ...
#include "Packet.h"             // Packet
#include "HostMgr.h"            // HostMgr
#include "Message.h"            // Message::Code, Message::Type
#include "EndpointManager.h"    // EndpointManager
#include "Endpoint2.h"          // Own interface


namespace ss
{



/* ****************************************************************************
*
* Endpoint2::Endpoint2 - Constructor
*/
Endpoint2::Endpoint2
(
	EndpointManager*  _epMgr,
	Type              _type,
	int               _id,
	const char*       _name,
	const char*       _alias,
	Host*             _host,
	unsigned short    _port,
	int               _rFd,
	int               _wFd
)
{
	epMgr            = _epMgr;
	type             = _type;
	id               = _id;
	host             = _host;
	rFd              = _rFd;
	wFd              = _wFd;
	port             = _port;
	state            = Unused;
	useSenderThread  = false;

	name             = NULL;
	alias            = NULL;
	
	memset(&sockin, 0, sizeof(sockin));

	if (_name != NULL)
		name  = strdup(_name);
	else
		name  = strdup("endpoint");

	if (_alias != NULL)
		alias = strdup(_alias);
	else
		alias = strdup("endpoint");
	
	msgsIn        = 0;
	msgsOut       = 0;
	msgsInErrors  = 0;
	msgsOutErrors = 0;
}



/* ****************************************************************************
*
* Endpoint2::
*/
Endpoint2::~Endpoint2()
{
	if (rFd != -1)
		close(rFd);
	if ((wFd != rFd) && (wFd != -1))
		close(wFd);

	if (name != NULL)
		free(name);

	if (alias != NULL)
		free(alias);
}



/* ****************************************************************************
*
* epMgrGet - 
*/
EndpointManager* Endpoint2::epMgrGet(void)
{
	return epMgr;
}



/* ****************************************************************************
*
* hostGet - 
*/
Host* Endpoint2::hostGet(void)
{
	// What if NULL ?

	return host;
}



/* ****************************************************************************
*
* hostname - 
*/
const char* Endpoint2::hostname(void)
{
	if (host == NULL)
		return "NullHost";

	return host->name;
}



/* ****************************************************************************
*
* typeGet - 
*/
Endpoint2::Type Endpoint2::typeGet(void)
{
	return type;
}



/* ****************************************************************************
*
* typeSet - 
*/
void Endpoint2::typeSet(Type _type)
{
	type = _type;
}



/* ****************************************************************************
*
* idGet - 
*/
int Endpoint2::idGet(void)
{
	return id;
}



/* ****************************************************************************
*
* idSet - 
*/
void Endpoint2::idSet(int _id)
{
	id = _id;
}



/* ****************************************************************************
*
* portSet - 
*/
void Endpoint2::portSet(unsigned short _port)
{
	if ((type == Endpoint2::Worker) && (_port != WORKER_PORT))
		LM_W(("Setting port for Worker to %d", _port));

	if ((type == Endpoint2::Controller) && (_port != CONTROLLER_PORT))
		LM_W(("Setting port for Controller to %d", _port));

	if ((type == Endpoint2::Spawner) && (_port != SPAWNER_PORT))
		LM_W(("Setting port for Spawner to %d", _port));

	port = _port;
}



/* ****************************************************************************
*
* portGet - 
*/
unsigned short Endpoint2::portGet(void)
{
	return port;
}



/* ****************************************************************************
*
* nameSet - 
*/
void Endpoint2::nameSet(const char* _name)
{
	if (name != NULL)
		free(name);

	name = strdup(_name);
}



/* ****************************************************************************
*
* nameGet - 
*/
const char* Endpoint2::nameGet(void)
{
	if (name == NULL)
		return "NULL name";

	return name;
}



/* ****************************************************************************
*
* aliasSet - 
*/
void Endpoint2::aliasSet(const char* _alias)
{
	if (alias != NULL)
		free(alias);

	alias = strdup(_alias);
}



/* ****************************************************************************
*
* aliasGet - 
*/
const char* Endpoint2::aliasGet(void)
{
	if (alias == NULL)
		return "NULL alias";

	return alias;
}



/* ****************************************************************************
*
* stateSet - 
*/
void Endpoint2::stateSet(State _state)
{
	state = _state;
}



/* ****************************************************************************
*
* stateGet - 
*/
Endpoint2::State Endpoint2::stateGet(void)
{
	return state;
}



/* ****************************************************************************
*
* rFdGet - 
*/
int Endpoint2::rFdGet(void)
{
	return rFd;
}



/* ****************************************************************************
*
* wFdGet - 
*/
int Endpoint2::wFdGet(void)
{
	return wFd;
}



/* ****************************************************************************
*
* okToSend - 
*/
Endpoint2::Status Endpoint2::okToSend(void)
{
	int             fds;
	fd_set          wFds;
	struct timeval  timeVal;
	int             tryh;
	int             tries = 30;

	for (tryh = 0; tryh < tries; tryh++)
	{
		timeVal.tv_sec  = 0;
		timeVal.tv_usec = 1000000;

		FD_ZERO(&wFds);
		FD_SET(rFd, &wFds);
	
		do
		{
			fds = select(rFd + 1, NULL, &wFds, NULL, &timeVal);
		} while ((fds == -1) && (errno == EINTR));

		if ((fds == 1) && (FD_ISSET(rFd, &wFds)))
			return OK;

		LM_W(("Problems to send to %s@%s (%d/%d secs)", name, host->name, tryh, tries));
	}

	LM_X(1, ("cannot write to '%s' - fd %d", name, rFd));
	return Timeout;
}



/* ****************************************************************************
*
* partSend - 
*/
Endpoint2::Status Endpoint2::partSend(void* dataP, int dataLen, const char* what)
{
	int    nb;
	int    tot  = 0;
	char*  data = (char*) dataP;
	Status s;

	while (tot < dataLen)
	{
		if ((s = okToSend()) != OK)
			LM_RE(s, ("Cannot write to '%s' (fd %d) (returning -2 as if it was a 'connection closed' ...)", name, wFd));
		
		nb = write(wFd, &data[tot], dataLen - tot);
		if (nb == -1)
			LM_RE(WriteError, ("error writing to '%s': %s", name, strerror(errno)));
		else if (nb == 0)
			LM_RE(WriteError, ("part-write written ZERO bytes to '%s' (total: %d)", name, tot));

		tot += nb;
	}

	LM_WRITES(name, what, dataP, dataLen, LmfByte);
	return OK;
}



/* ****************************************************************************
*
* ack - 
*/
Endpoint2::Status Endpoint2::ack(Message::MessageCode code, void* data, int dataLen)
{
	return send(Message::Ack, code, data, dataLen);
}



/* ****************************************************************************
*
* send - 
*/
Endpoint2::Status Endpoint2::send
(
	Message::MessageType  type,
	Message::MessageCode  code,
	void*                 data,
	int                   dataLen,
	Packet*               packetP
)
{
	Status           s;
	Message::Header  header;

	if (code == Message::Die)
		LM_W(("%s sending a Die to %s", epMgr->me->name, name));


	//
	// Preparing header
	//
	memset(&header, 0, sizeof(header));

	header.code        = code;
	header.type        = type;
	header.magic       = 0xFEEDC0DE;

	if ((dataLen != 0) && (data != NULL))
		header.dataLen = dataLen;

	if ((packetP != NULL) && (packetP->message->ByteSize() != 0))
		header.gbufLen = packetP->message->ByteSize();

	if (packetP && (packetP->buffer != 0))
		header.kvDataLen = packetP->buffer->getSize();



	//
	// Sending header
	//
	s = partSend(&header, sizeof(header), "header");
	if (s != OK)
		LM_RE(s, ("partSend:header(%s): %s", name, status(s)));


	
	//
	// Sending raw data
	//
	if ((dataLen != 0) && (data != NULL))
	{
		s = partSend(data, dataLen, "msg data");
		if (s != OK)
			LM_RE(s, ("partSend:data(%s): %s", name, status(s)));
	}



	//
	// Sending Google Protocol Buffer
	//
	if ((packetP != NULL) && (packetP->message->ByteSize() != 0))
	{
		char* outputVec;

		outputVec = (char*) malloc(header.gbufLen + 2);
		if (outputVec == NULL)
			LM_XP(1, ("malloc(%d)", header.gbufLen));

		if (packetP->message->SerializeToArray(outputVec, header.gbufLen) == false)
			LM_X(1, ("SerializeToArray failed"));

		s = partSend(outputVec, packetP->message->ByteSize(), "Google Protocol Buffer");
		free(outputVec);
		if (s != packetP->message->ByteSize())
			LM_RE(s, ("partSend:GoogleProtocolBuffer(): %s", status(s)));
	}

	if (packetP && (packetP->buffer != 0))
	{
		s = partSend(packetP->buffer->getData(), packetP->buffer->getSize(), "KV data");
		if (s != OK)
			LM_RE(s, ("partSend returned %d and not the expected %d", s, packetP->buffer->getSize()));
	}

	if (packetP != NULL)
	{
	   engine::MemoryManager::shared()->destroyBuffer( packetP->buffer );
	   delete packetP;
	}
	
	return OK;
}



/* ****************************************************************************
*
* partRead - 
*/
Endpoint2::Status Endpoint2::partRead(void* vbuf, long bufLen, long* bufLenP, const char* what)
{
	ssize_t  tot = 0;
	Status   s;
	char*    buf = (char*) vbuf;

	LM_M(("Reading %d bytes of data", bufLen));
	while (tot < bufLen)
	{
		ssize_t nb;

		s = msgAwait(0, 500000);
		if (s != OK)
			LM_RE(s, ("msgAwait(%s): %s", name, status(s)));

		LM_M(("Reading from fd %d", rFd));
		nb = read(rFd, (void*) &buf[tot] , bufLen - tot);
		LM_M(("read returned %d", nb));
		if (nb == -1)
		{
			if (errno == EBADF)
				LM_RE(ConnectionClosed, ("read(%s): %s (treating as Connection Closed)", name, strerror(errno)));

			LM_RE(ConnectionClosed, ("read(%s): %s", name, strerror(errno)));
		}
		else if (nb == 0)
			LM_RE(ConnectionClosed, ("Connection closed by '%s'", name));

		tot += nb;
	}

	if (bufLenP == NULL)
		LM_X(1, ("Got called with NULL buffer length pointer. This is a programmer's bug and must be fixed. Right now."));

	*bufLenP = tot;
	LM_M(("Set *bufLenP to %d", *bufLenP));

	LM_M(("Read %d bytes of data", tot));
	LM_READS(name, what, buf, tot, LmfByte);

	return OK;
}



/* ****************************************************************************
*
* receive - 
*/
Endpoint2::Status Endpoint2::receive(Message::Header* headerP, void** dataPP, long* dataLenP, Packet* packetP)
{
	Status s;
	long   bufLen;
	long   totalBytesReadExceptHeader = 0;

	if ((type == Listener) || (type == WebListener))
		LM_X(1, ("Listener endpoint - should never get here ..."));

	*dataPP = NULL;

	LM_M(("Calling partRead for header"));
	s = partRead(headerP, sizeof(Message::Header), &bufLen, "Header");
	if (s != OK)
		LM_RE(s, ("partRead::Header(%s): %s", name, status(s)));

	LM_M(("Read header of %d bytes (next is %d bytes of data)", bufLen, headerP->dataLen));
	if (headerP->dataLen != 0)
	{
		*dataPP = calloc(1, headerP->dataLen);

		LM_M(("Calling partRead for data"));
		s = partRead(*dataPP, headerP->dataLen, &bufLen, "Binary Data");
		if (s != OK)
		{
			free(*dataPP);
			LM_RE(s, ("partRead::Data(%s): %s", name, status(s)));
		}
		LM_M(("Read %d bytes of data", bufLen));
		totalBytesReadExceptHeader += bufLen;
	}

	if (headerP->gbufLen != 0)
	{
		char* dataP = (char*) calloc(1, headerP->gbufLen + 1);

		s = partRead(dataP, headerP->gbufLen, &bufLen, "Google Protocol Buffer");
		if (s != OK)
		{
			free(dataP);
			if (*dataPP != NULL)
				free(*dataPP);
			LM_RE(s, ("partRead::GoogleProtocolBuffer(%s): %s", name, status(s)));
		}
		LM_M(("Read %d bytes of google data", bufLen));

		packetP->message->ParseFromArray(dataP, headerP->gbufLen);
		if (packetP->message->IsInitialized() == false)
			LM_X(1, ("Error parsing Google Protocol Buffer of %d bytes because a message %s is not initialized!",
					 headerP->gbufLen, ss::Message::messageCode(headerP->code)));
		free(dataP);
		totalBytesReadExceptHeader += bufLen;
	}

	if (headerP->kvDataLen != 0)
	{
		char         kvName[128];
		static int   bIx = 0;

		sprintf(kvName, "%s:%d", name, bIx);
		++bIx;

		packetP->buffer = engine::MemoryManager::shared()->newBuffer(kvName, headerP->kvDataLen, engine::Buffer::output);

		char*  kvBuf  = packetP->buffer->getData();
		long   nb     = 0;

		s = partRead(kvBuf, headerP->kvDataLen, &nb, "Key-Value Data");
		if (s != OK)
			LM_RE(s, ("partRead::kvData(%s): %s", kvName, status(s)));
		LM_M(("Read %d bytes of KV data", nb));

		packetP->buffer->setSize(nb);
		totalBytesReadExceptHeader += bufLen;
	}

	if (dataLenP == NULL)
		LM_X(1, ("Got called with NULL buffer length pointer. This is a programmer's bug and must be fixed. Right now."));
	*dataLenP = totalBytesReadExceptHeader;

	return OK;
}



/* ****************************************************************************
*
* connect - 
*/
Endpoint2::Status Endpoint2::connect(void)
{
	struct hostent*     hp;
	struct sockaddr_in  peer;

	if (host == NULL)
		LM_RE(NullHost, ("Cannot connect to endpoint '%s' with NULL host!", name));
	if (port == 0)
		LM_RE(NullPort, ("Cannot connect to '%s@%s' - port is ZERO", name, host->name));

	LM_T(LmtConnect, ("Trying to connect to %s at %s:%d", name, host->name, port));

	if ((hp = gethostbyname(host->name)) == NULL)
		LM_RE(GetHostByNameError, ("gethostbyname(%s): %s", host->name, strerror(errno)));

	if ((rFd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
		LM_RE(SocketError, ("socket: %s", strerror(errno)));
	
	wFd = rFd;

	memset((char*) &peer, 0, sizeof(peer));

	peer.sin_family      = AF_INET;
	peer.sin_addr.s_addr = ((struct in_addr*) (hp->h_addr))->s_addr;
	peer.sin_port        = htons(port);

	if (::connect(wFd, (struct sockaddr*) &peer, sizeof(peer)) == -1)
	{
		usleep(50000);
		if (::connect(wFd, (struct sockaddr*) &peer, sizeof(peer)) == -1)
		{
			close(rFd);
			rFd = -1;
			wFd = -1;
			LM_RE(ConnectError, ("Cannot connect to %s, port %d", host->name, port));
		}
	}

#if 0
	int bufSize = 64 * 1024 * 1024;

	int s;
	s = setsockopt(wFd, SOL_SOCKET, SO_RCVBUF, &bufSize, sizeof(bufSize));
	if (s != 0)
		LM_X(1, ("setsockopt(SO_RCVBUF): %s", strerror(errno)));
	s = setsockopt(fd, SOL_SOCKET, SO_SNDBUF, &bufSize, sizeof(bufSize));
	if (s != 0)
		LM_X(1, ("setsockopt(SO_SNDBUF): %s", strerror(errno)));

	// Disable the Nagle (TCP No Delay) algorithm
	int flag = 1;
	s = setsockopt(wFd, IPPROTO_TCP, TCP_NODELAY, (char*) &flag, sizeof(flag));
	if (s != 0)
		LM_X(1, ("setsockopt(TCP_NODELAY): %s", strerror(errno)));
#endif

	LM_T(LmtConnect, ("connected to '%s', port %d on fd %d", host->name, port, wFd));

	return OK;
}



/* ****************************************************************************
*
* msgAwait - 
*/
Endpoint2::Status Endpoint2::msgAwait(int secs, int usecs)
{
	struct timeval  tv;
	struct timeval* tvP;
	int             fds;
	fd_set          rFds;

	if (secs == -1)
		tvP = NULL;
	else
	{
		tv.tv_sec  = secs;
		tv.tv_usec = usecs;

		tvP        = &tv;
	}

	do
	{
		FD_ZERO(&rFds);
		FD_SET(rFd, &rFds);
		fds = select(rFd + 1, &rFds, NULL, NULL, tvP);
	} while ((fds == -1) && (errno == EINTR));

	if (fds == -1)
		LM_RP(SelectError, ("select"));
	else if (fds == 0)
		LM_RE(Timeout, ("timeout"));
	else if ((fds > 0) && (!FD_ISSET(rFd, &rFds)))
		LM_RE(Error, ("some other fd has a read pending - this is impossible !"));
	else if ((fds > 0) && (FD_ISSET(rFd, &rFds)))
		return OK;

	LM_X(1, ("Other very strange error"));

	return Error;
}



/* ****************************************************************************
*
* msgTreat - 
*/
Endpoint2::Status Endpoint2::msgTreat(void)
{
	Message::Header      header;
	void*                dataP    = NULL;
	long                 dataLen  = 0;
	Packet               packet(Message::Unknown);
	Endpoint2::Status    s;

	if (type == Listener)
		return msgTreat2();
	if (type == WebListener)
		return msgTreat2();
	if (type == WebWorker)
		return msgTreat2();
	
	LM_M(("Reading a message from '%s' '%s@%s', type '%s'", name, alias, host->name, typeName()));
	s = receive(&header, &dataP, &dataLen, &packet);
	if (s != 0)
		LM_RE(s, ("receive error '%s'", status(s)));

	if (type == Unhelloed)
		return msgTreat2(&header, dataP, dataLen, &packet);

	switch (header.code)
	{
	default:
		LM_M(("Cannot treat '%s' '%s' (code %d), passing it to msgTreat2", messageCode(header.code), messageType(header.type), header.code));
		s = msgTreat2(&header, dataP, dataLen, &packet);
		if (s != OK)
			LM_RE(s, ("msgTreat2: %s", s));
		break;
	}

	return OK;
}



/* *******************************************************************************
*
* typeName - 
*/
const char* Endpoint2::typeName(Type type)
{
	switch (type)
	{
	case Unhelloed:      return "Unhelloed";
	case Worker:         return "Worker";
	case Controller:     return "Controller";
	case Spawner:        return "Spawner";
	case Supervisor:     return "Supervisor";
	case Delilah:        return "Delilah";
	case Killer:         return "Killer";
	case Setup:          return "Setup";
	case Listener:       return "Listener";
	case Sender:         return "Sender";
	case CoreWorker:     return "CoreWorker";
	case ThreadReader:   return "ThreadReader";
	case ThreadSender:   return "ThreadSender";
	case WebListener:    return "WebListener";
	case WebWorker:      return "WebWorker";
	}

	return "Unknown";
}



/* *******************************************************************************
*
* typeName - 
*/
const char* Endpoint2::typeName(void)
{
	return typeName(type);
}



/* *******************************************************************************
*
* status - 
*/
const char* Endpoint2::status(Status s)
{
	switch (s)
	{
	case OK:                   return "OK";
	case NotImplemented:       return "NotImplemented";

	case NullAlias:            return "NullAlias";
	case BadAlias:             return "BadAlias";
	case NullHost:             return "NullHost";
	case BadHost:              return "BadHost";
	case NullPort:             return "NullPort";
	case Duplicated:           return "Duplicated";
	case KillError:            return "KillError";
	case NotHello:             return "NotHello";
	case NotAck:               return "NotAck";
	case NotMsg:               return "NotMsg";

	case Error:                return "Error";
	case ConnectError:         return "ConnectError";
	case AcceptError:          return "AcceptError";
	case NotListener:          return "NotListener";
	case SelectError:          return "SelectError";
	case SocketError:          return "SocketError";
	case GetHostByNameError:   return "GetHostByNameError";
	case BindError:            return "BindError";
	case ListenError:          return "ListenError";
	case ReadError:            return "ReadError";
	case WriteError:           return "WriteError";
	case Timeout:              return "Timeout";
	case ConnectionClosed:     return "ConnectionClosed";
	}

	return "Unknown Status";
}



/* ****************************************************************************
*
* send - 
*/
size_t Endpoint2::send(PacketSenderInterface* psi, Message::MessageCode code, Packet* packetP)
{
	// semTake(jobQueueSem);
	// jobEnqueue(pi, code, packetP);
	// semGive(jobQueueSem);
	//
	// The thread will notice the packet at next run-timeout
	//
	return 0;
}



/* ****************************************************************************
*
* run - 
*/
void Endpoint2::run()
{
	LM_M(("Endpoint '%s@%s' is running", name, host->name));
	while (1)
		msgTreat();
}



/* ****************************************************************************
*
* hello - send hello and await ack, with timeout
*/
Endpoint2::Status Endpoint2::hello(int secs, int usecs)
{
	Message::HelloData   h;
	Message::Header      header;
	void*                dataP = &header;
	long                 dataLen;
	Endpoint2::Status    s;
	Packet               packet(Message::Hello);

	strncpy(h.name,  epMgr->me->nameGet(),  sizeof(h.name)  - 1);
	strncpy(h.ip,    epMgr->me->hostname(), sizeof(h.ip)    - 1);
	strncpy(h.alias, epMgr->me->aliasGet(), sizeof(h.alias) - 1);

	h.type      = epMgr->me->typeGet();
	h.workers   = -1;     // no longer used?
	h.coreNo    = -1;     // no longer used?
	h.workerId  = -1;     // no longer used?

	if ((s = send(Message::Msg, Message::Hello, &h, sizeof(h))) != OK)
		LM_RE(s, ("send(Hello Msg): %s", status(s)));

	if ((s = msgAwait(secs, usecs)) != OK)
		LM_RE(s, ("msgAwait: %s", status(s)));

	dataLen = sizeof(h);
	if ((s = receive(&header, &dataP, &dataLen, &packet)) != OK)
		LM_RE(s, ("receive: %s", status(s)));

	if (header.code != Message::Hello)
		LM_RE(NotHello, ("Wanted an ack for a hello, got a '%s' for '%s'", Message::messageType(header.type), Message::messageCode(header.code)));
	if (header.type != Message::Ack)
		LM_RE(NotAck, ("Wanted an ack for a hello, got a '%s' for '%s'",   Message::messageType(header.type), Message::messageCode(header.code)));

	return OK;
}



/* ****************************************************************************
*
* die - send die to endpoint and await death, with timeout
*/
Endpoint2::Status Endpoint2::die(int secs, int usecs)
{
	char    c;
	int     nb;
	Status  s;

	if ((s = send(Message::Msg, Message::Die)) != OK)
		LM_RE(s, ("send(Die Msg): %s", status(s)));

	if ((s = msgAwait(secs, usecs)) != OK)
		LM_RE(s, ("msgAwait: %s", status(s)));

	nb = read(rFd, &c, 1);
	if (nb == -1)
		LM_RE(ReadError, ("Expected to read 0 bytes, meaning 'Connection Closed', but read() returned -1: %s", strerror(errno)));
	else if (nb != 0)
		LM_RE(KillError, ("Endpoint not dead, even though a Die message was sent!"));

	LM_TODO(("This endpoint must be removed from EndpointManager, right?"));

	return OK;
}

}
