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
*
* ToDo
*   - remove 'alias' and 'name' completely
*   - create method 'name()' to return ("%s%d@%s", typeName(), id, hostname())
*     AND if id == -1, ("%s%@%s", typeName(), hostname())
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

#include "MemoryTags.h"         // MemoryOutputDisk

namespace ss
{



/* ****************************************************************************
*
* writerThread - 
*/
static void* writerThread(void* vP)
{
	Endpoint2* ep = (Endpoint2*) vP;

	LM_M(("writer thread for endpoint %s@%s is running (REAL wFd: %d))", ep->nameGet(), ep->hostGet()->name, ep->wFdGet()));

	while (1)
	{
		JobQueue::Job* job;

		while (1)
		{
			job = ep->jobQ->pop();
			if (job != NULL)
				break;
			sleep(1);
		}

		ep->send(job->packetP->msgType, job->packetP->msgCode, job->packetP->dataP, job->packetP->dataLen, job->packetP);
		delete job->packetP;
		if (job->psi != NULL)
			job->psi->notificationSent(0, true);
	}

	return NULL;
}



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
		name  = strdup(typeName());
	
	if (_alias != NULL)
		alias = strdup(_alias);
	else
		alias = strdup("endpoint");
	
	msgsIn        = 0;
	msgsOut       = 0;
	msgsInErrors  = 0;
	msgsOutErrors = 0;

	jobQ = new JobQueue();
}



/* ****************************************************************************
*
* Endpoint2::~Endpoint2 - 
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
* stateName - 
*/
const char* Endpoint2::stateName(void)
{
	switch (state)
	{
	case Unused:                    return "Unused";
	case Ready:                     return "Ready";
	case Disconnected:              return "Disconnected";
	case ScheduledForRemoval:       return "ScheduledForRemoval";
	}

	return "Unknown";
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
		FD_SET(wFd, &wFds);
	
		LM_M(("Hanging on a select(write) on fd %d", wFd));
		do
		{
			fds = select(wFd + 1, NULL, &wFds, NULL, &timeVal);
		} while ((fds == -1) && (errno == EINTR));

		if ((fds == 1) && (FD_ISSET(wFd, &wFds)))
			return OK;

		LM_W(("Problems to send to %s@%s (%d/%d secs)", name, host->name, tryh, tries));
	}

	LM_X(1, ("cannot write to '%s' - fd %d", name, wFd));
	return Timeout;
}



/* ****************************************************************************
*
* partWrite - 
*/
Endpoint2::Status Endpoint2::partWrite(void* dataP, int dataLen, const char* what)
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
void Endpoint2::ack(Message::MessageCode code, void* data, int dataLen)
{
	Packet* packet = new Packet(Message::Ack, code, data, dataLen);
	send(NULL, packet);
}



/* ****************************************************************************
*
* send - 
*/
void Endpoint2::send(PacketSenderInterface* psi, Packet* packetP)
{
	if ((host == epMgr->me->host) && (type == epMgr->me->typeGet()))
	{
		LM_M(("Looping back a packet meant for myself (and calling psi->notificationSent ...)"));
		if (epMgr->packetReceiver == NULL)
			LM_X(1, ("Np packetReceiver - no real use to contiune, this is a SW bug!"));
		epMgr->packetReceiver->_receive(packetP);
		if (psi)
			psi->notificationSent(0, true);
	}
	else
		jobQ->push(psi, packetP);
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
		LM_W(("Sending a Die '%s' to %s@%s", messageType(type), nameGet(), hostname()));
	else
		LM_M(("Sending a '%s' '%s' to %s@%s", messageCode(code), messageType(type), nameGet(), hostname()));
		


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
	s = partWrite(&header, sizeof(header), "header");
	if (s != OK)
		LM_RE(s, ("partWrite:header(%s): %s", name, status(s)));


	
	//
	// Sending raw data
	//
	if ((dataLen != 0) && (data != NULL))
	{
		s = partWrite(data, dataLen, "msg data");
		if (s != OK)
			LM_RE(s, ("partWrite:data(%s): %s", name, status(s)));
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

		s = partWrite(outputVec, packetP->message->ByteSize(), "Google Protocol Buffer");
		free(outputVec);
		if (s != packetP->message->ByteSize())
			LM_RE(s, ("partWrite:GoogleProtocolBuffer(): %s", status(s)));
	}

	if (packetP && (packetP->buffer != 0))
	{
		s = partWrite(packetP->buffer->getData(), packetP->buffer->getSize(), "KV data");
		if (s != OK)
			LM_RE(s, ("partWrite returned %d and not the expected %d", s, packetP->buffer->getSize()));
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

	while (tot < bufLen)
	{
		ssize_t nb;

		s = msgAwait(0, 500000, what);
		if (s != OK)
			LM_RE(s, ("msgAwait(%s): %s, expecting '%s' from %s@%s", name, status(s), what, name, hostname()));

		nb = read(rFd, (void*) &buf[tot] , bufLen - tot);
		if (nb == -1)
		{
			if (errno == EBADF)
				LM_RE(ConnectionClosed, ("read(%s): %s (treating as Connection Closed), expecting '%s' from %s@%s", name, strerror(errno), what, name, hostname()));

			LM_RE(ConnectionClosed, ("read(%s): %s, expecting '%s' from %s@%s", name, strerror(errno), what, name, hostname()));
		}
		else if (nb == 0)
			LM_RE(ConnectionClosed, ("Connection closed by '%s', expecting '%s' from %s@%s", name, what, name, hostname()));

		tot += nb;
	}

	if (bufLenP == NULL)
		LM_X(1, ("Got called with NULL buffer length pointer. This is a programmer's bug and must be fixed. Right now. (Expecting '%s' from %s@%s)", what, name, hostname()));

	*bufLenP = tot;
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

	s = partRead(headerP, sizeof(Message::Header), &bufLen, "Header");
	if (s != OK)
		LM_RE(s, ("partRead: %s, expecting 'Header' from '%s@%s'", status(s), name, hostname()));

	LM_M(("Read '%s' '%s' header of %d bytes from '%s@%s'", messageCode(headerP->code), messageType(headerP->type), bufLen, nameGet(), hostname()));
	if (headerP->dataLen != 0)
	{
		*dataPP = calloc(1, headerP->dataLen);

		s = partRead(*dataPP, headerP->dataLen, &bufLen, "Binary Data");
		if (s != OK)
		{
			free(*dataPP);
			LM_RE(s, ("partRead: %s, expecting '%d RAW DATA bytes' from '%s@%s'", status(s), headerP->dataLen,  name, hostname()));
		}
		LM_M(("Read %d bytes of RAW DATA from '%s@%s'", bufLen, nameGet(), hostname()));
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
			LM_RE(s, ("partRead: %s, expecting '%d bytes of Google Protocol Buffer data' from '%s@%s'", status(s), headerP->gbufLen, name, hostname()));
		}
		LM_M(("Read %d bytes of GOOGLE DATA from '%s@%s'", bufLen, nameGet(), hostname()));

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

		packetP->buffer = engine::MemoryManager::shared()->newBuffer(kvName, headerP->kvDataLen, ss::MemoryOutputDisk  );

		char*  kvBuf  = packetP->buffer->getData();
		long   nb     = 0;

		s = partRead(kvBuf, headerP->kvDataLen, &nb, "Key-Value Data");
		if (s != OK)
			LM_RE(s, ("partRead: %s, expecting '%d bytes of KV DATA (%s)' from '%s@%s'", status(s), headerP->kvDataLen, kvName, name, hostname()));
		LM_M(("Read %d bytes of KV DATA from '%s@%s'", nb, nameGet(), hostname()));

		packetP->buffer->setSize(nb);
		totalBytesReadExceptHeader += bufLen;
	}

	if (dataLenP == NULL)
		LM_X(1, ("Got called with NULL buffer length pointer. This is a programmer's bug and must be fixed. Right now. Come on, do it!"));
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
Endpoint2::Status Endpoint2::msgAwait(int secs, int usecs, const char* what)
{
	struct timeval  tv;
	struct timeval* tvP;
	int             fds;
	fd_set          rFds;

	do
	{
		if (secs == -1)
			tvP = NULL;
		else
		{
			tv.tv_sec  = secs;
			tv.tv_usec = usecs;

			tvP        = &tv;
		}

		FD_ZERO(&rFds);
		FD_SET(rFd, &rFds);
		fds = select(rFd + 1, &rFds, NULL, NULL, tvP);
	} while ((fds == -1) && (errno == EINTR));

	if (fds == -1)
		LM_RP(SelectError, ("select error awaiting '%s' from '%s@%s", what, name, hostname()));
	else if (fds == 0)
		LM_RE(Timeout, ("timeout awaiting '%s' from '%s@%s'", what, name, hostname()));
	else if ((fds > 0) && (!FD_ISSET(rFd, &rFds)))
		LM_RE(Error, ("some other fd has a read pending - this is impossible ! (awaiting '%s' from '%s@%s')", what, name, hostname()));
	else if ((fds > 0) && (FD_ISSET(rFd, &rFds)))
		return OK;

	LM_X(1, ("Very strange error awaiting '%s' from '%s@%s'", what, name, hostname()));

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
	Message::HelloData*  helloP;

	LM_M(("Treating a message from %s@%s", name, host->name));

	if (type == Listener)
		return msgTreat2();
	if (type == WebListener)
		return msgTreat2();
	if (type == WebWorker)
		return msgTreat2();

	LM_M(("Reading a message from '%s@%s'", name, host->name));
	s = msgAwait(-1, -1, "Incoming message");
	if (s != OK)
		LM_E(("msgAwait: %s - what do I do ... ?", status(s)));

	s = receive(&header, &dataP, &dataLen, &packet);
	if (s != 0)
		LM_RE(s, ("receive error '%s'", status(s)));

	if (type == Unhelloed)
		return msgTreat2(&header, dataP, dataLen, &packet);

	switch (header.code)
	{
	case Message::Hello:
		helloP = (Message::HelloData*) dataP;
		s = helloDataSet((Type) helloP->type, helloP->name, helloP->alias);
		if (s != OK)
		{
			stateSet(ScheduledForRemoval);
			LM_RE(s, ("Bad hello data"));
		}

		if (header.type == Message::Msg)
		{
			if ((s = helloSend(Message::Ack)) != OK)
			{
				stateSet(ScheduledForRemoval);
				LM_RE(s, ("helloSend error"));
			}
		}

		if (useSenderThread == true)
		{
			pthread_t tid;
			int       ps;

			LM_M(("Creating writer thread for endpoint %s@%s", name, host->name));
			if ((ps = pthread_create(&tid, NULL, writerThread, this)) != 0)
			{
				LM_E(("pthread_create returned %d for %s@%s", ps, name, host->name));
				return PThreadError;
			}
		}
		break;

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
	case NotImplemented:       return "Not Implemented";

	case NullAlias:            return "Null Alias";
	case BadAlias:             return "Bad Alias";
	case NullHost:             return "Null Host";
	case BadHost:              return "Bad Host";
	case NullPort:             return "Null Port";
	case Duplicated:           return "Duplicated";
	case KillError:            return "Kill Error";
	case NotHello:             return "Not Hello";
	case NotAck:               return "Not an Ack";
	case NotMsg:               return "Not a Msg";

	case Error:                return "Error";
	case ConnectError:         return "Connect Error";
	case AcceptError:          return "Accept Error";
	case NotListener:          return "Not a Listener";
	case SelectError:          return "Select Error";
	case SocketError:          return "Socket Error";
	case GetHostByNameError:   return "Get Host By Name Error";
	case BindError:            return "Bin dError";
	case ListenError:          return "Listen Error";
	case ReadError:            return "Read Error";
	case WriteError:           return "Write Error";
	case Timeout:              return "Timeout";
	case ConnectionClosed:     return "Connection Closed";
	case PThreadError:         return "Thread Error";
	}

	return "Unknown Status";
}



/* ****************************************************************************
*
* run - 
*/
void Endpoint2::run(void)
{
	LM_M(("Endpoint '%s@%s' reader thread is running", name, host->name));

	while (1)
		msgTreat();
}



/* ****************************************************************************
*
* hello - send hello message
*/
Endpoint2::Status Endpoint2::hello(int secs, int usecs)
{
	Message::HelloData   h;
	Message::Header      header;
	void*                dataP   = &header;
	long                 dataLen = sizeof(h);
	Endpoint2::Status    s;
	Packet*              packet = new Packet(Message::Msg, Message::Hello, &h, dataLen);

	strncpy(h.name,  epMgr->me->nameGet(),  sizeof(h.name)  - 1);
	strncpy(h.ip,    epMgr->me->hostname(), sizeof(h.ip)    - 1);
	strncpy(h.alias, epMgr->me->aliasGet(), sizeof(h.alias) - 1);

	h.type      = epMgr->me->typeGet();
	h.workers   = -1;     // no longer used?
	h.coreNo    = -1;     // no longer used?
	h.workerId  = -1;     // no longer used?

	send(NULL, packet);

	if ((s = msgAwait(secs, usecs, "Hello header")) != OK)
		LM_RE(s, ("msgAwait: %s", status(s)));

	if ((s = receive(&header, &dataP, &dataLen, packet)) != OK)
		LM_RE(s, ("receive: %s", status(s)));

	if (header.code != Message::Hello)
		LM_RE(NotHello, ("Wanted an ack for a hello, got a '%s' for '%s'",
						 Message::messageType(header.type), Message::messageCode(header.code)));
	if (header.type != Message::Ack)
		LM_RE(NotAck, ("Wanted an ack for a hello, got a '%s' for '%s'",
					   Message::messageType(header.type), Message::messageCode(header.code)));

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

	if ((s = msgAwait(secs, usecs, "Connection Closed")) != OK)
		LM_RE(s, ("msgAwait: %s", status(s)));

	nb = read(rFd, &c, 1);
	if (nb == -1)
		LM_RE(ReadError, ("Expected to read 0 bytes, meaning 'Connection Closed', but read() returned -1: %s", strerror(errno)));
	else if (nb != 0)
		LM_RE(KillError, ("Endpoint not dead, even though a Die message was sent!"));

	LM_TODO(("This endpoint must be removed from EndpointManager, right?"));

	return OK;
}



/* ****************************************************************************
*
* - 
*/
bool Endpoint2::threaded(void)
{
	return useSenderThread;
}



/* ****************************************************************************
*
* helloDataSet - 
*/
Endpoint2::Status Endpoint2::helloDataSet(Type _type, const char* _name, const char* _alias)
{
	if (type != Unhelloed)
	{
		if (type != _type)
			LM_W(("Got a Hello from %s@%s, saying he's of '%s' type, when endpoint says '%s'", name, hostname(), typeName(_type), typeName()));
	}

	type = _type;
	nameSet(_name);
	aliasSet(_alias);

	LM_M(("Set type to %d (%s), name to '%s' and alias to '%s' AND state to Ready", type, typeName(type), name, alias));

	state = Ready;
	return OK;
}



/* ****************************************************************************
*
* helloSend - 
*/
Endpoint2::Status Endpoint2::helloSend(Message::MessageType type)
{
	Message::HelloData hello;

	memset(&hello, 0, sizeof(hello));

	strncpy(hello.name,   epMgr->me->nameGet(),    sizeof(hello.name));
	strncpy(hello.ip,     epMgr->me->hostname(),   sizeof(hello.ip));
	strncpy(hello.alias,  epMgr->me->aliasGet(),   sizeof(hello.alias));

	hello.type     = epMgr->me->typeGet();
	hello.coreNo   = 0;
	hello.workerId = 0;

	LM_T(LmtWrite, ("sending hello %s to '%s' (my name: '%s', my type: '%s')", messageType(type), name, hello.name, epMgr->me->typeName()));

	return send(type, Message::Hello, &hello, sizeof(hello));
}

}
