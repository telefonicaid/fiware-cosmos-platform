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
Endpoint2::Endpoint2(EndpointManager* _epMgr, Type _type, const char* _name, const char* _alias, Host* _host, unsigned short _port, int _rFd, int _wFd)
{
	epMgr            = _epMgr;
	type             = _type;
	host             = _host;
	rFd              = _rFd;
	wFd              = _wFd;
	port             = _port;
	state            = Usused;
	useSenderThread  = false;

	name             = NULL;
	alias            = NULL;
	
	memset(&sockin, 0, sizeof(sockin));

	if (_name != NULL)
		name  = strdup(_name);
	if (_alias != NULL)
		alias = strdup(_alias);

	if ((type == Listener) || (type == WebListener))
		listenerPrepare();
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
* rFdGet - 
*/
int Endpoint2::rFdGet(void)
{
	return rFd;
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
* processLookup - 
*/
static Process* processLookup(ProcessVector* procVec, const char* alias)
{
	for (int ix = 0; ix < procVec->processes; ix++)
	{
		Process* p = &procVec->processV[ix];

		if (strcmp(p->alias, alias) == 0)
			return p;
	}

	return NULL;
}



/* ****************************************************************************
*
* helloDataAdd - 
*
* Controller and Worker endpoints should all be added with the type 'Unknown'.
* When the Hello data arrives, a check will be performed to see whether the 
* connection is valid (not occupied).
*/
Endpoint2::Status Endpoint2::helloDataAdd(Endpoint2::Type _type, const char* _name, const char* _alias)
{
	if ((_type == Endpoint2::Controller) || (_type == Endpoint2::Worker))
	{
		Process*  proc;

		if (_alias == NULL)
		{
			state = ScheduledForRemoval;
			LM_RE(NullAlias, ("NULL alias"));
		}

		if ((proc = processLookup(epMgr->procVec, _alias)) == NULL)
		{
			state = ScheduledForRemoval;
			LM_RE(BadAlias, ("alias '%s' not found in process vector", _alias));
		}

		if (epMgr->hostMgr->match(host, proc->host) == false)  // Host could become a class and to its own matching
		{
			state = ScheduledForRemoval;
			LM_RE(BadHost, ("The host for alias '%s' must be '%s'. This endpoints host ('%s') is incorrect",
							_alias, hostname(), proc->host));
		}

		if (epMgr->lookup(_type, _alias) != NULL)
		{
			state = ScheduledForRemoval;
			LM_RE(Duplicated, ("Duplicated process"));
		}

		if (_type == Endpoint2::Controller)
			epMgr->controller = this;
	}

	type = _type;
	nameSet(_name);
	aliasSet(_alias);

	// Create sender thread right here ? Guess so ...
	return OK;
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

	return OK;
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
Endpoint2::Status Endpoint2::partRead(void* vbuf, long bufLen, long* bufLenP)
{
	ssize_t  tot = 0;
	Status   s;
	const char* buf = (const char*) vbuf;

	while (tot < bufLen)
	{
		ssize_t nb;

		s = msgAwait(0, 500000);
		if (s != 1)
			LM_RE(s, ("msgAwait(%s): %s", name, status(s)));

		nb = read(rFd, (void*) &buf[tot] , bufLen - tot);
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

	if (bufLenP)
		*bufLenP = tot;

	return OK;
}



/* ****************************************************************************
*
* receive - 
*/
Endpoint2::Status Endpoint2::receive(Message::Header* headerP, void** dataPP, int* dataLenP, Packet* packetP)
{
	Status s;

	if (type == Listener)
	{
		if (accept(true) == NULL)
			return AcceptError;
		else
			return OK;
	}

	*dataPP = NULL;

	s = partRead(headerP, sizeof(Message::Header));
	if (s != OK)
		LM_RE(s, ("partRead::Header(%s): %s", name, status(s)));

	if (headerP->dataLen != 0)
	{
		*dataPP = calloc(1, headerP->dataLen);

		s = partRead(*dataPP, headerP->dataLen);
		if (s != OK)
		{
			free(*dataPP);
			LM_RE(s, ("partRead::Data(%s): %s", name, status(s)));
		}
	}

	if (headerP->gbufLen != 0)
	{
		char* dataP = (char*) calloc(1, headerP->gbufLen + 1);

		s = partRead(dataP, headerP->gbufLen);
		if (s != OK)
		{
			free(dataP);
			if (*dataPP != NULL)
				free(*dataPP);
			LM_RE(s, ("partRead::GoogleProtocolBuffer(%s): %s", name, status(s)));
		}

		packetP->message->ParseFromArray(dataP, headerP->gbufLen);
		if (packetP->message->IsInitialized() == false)
			LM_X(1, ("Error parsing Google Protocol Buffer of %d bytes because a message %s is not initialized!",
					 headerP->gbufLen, ss::Message::messageCode(headerP->code)));
		free(dataP);
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

		s = partRead(kvBuf, headerP->kvDataLen, &nb);
		if (s != OK)
			LM_RE(s, ("partRead::kvData(%s): %s", kvName, status(s)));

		packetP->buffer->setSize(nb);
	}

	return OK;
}



/* ****************************************************************************
*
* connect - 
*/
Endpoint2* Endpoint2::connect(bool addToEpVec)
{
	if (port == 0)
		LM_RE(NULL, ("Cannot connect to '%s@%s' - port is ZERO", name, host->name));

	// Implement iomConnect here
	// if (addToEpVec) - add the accepted endpoint to endpoint vector with type Anonymous

	return NULL;
}



/* ****************************************************************************
*
* ip2string - convert integer ip address to string
*/
static void ip2string(int ip, char* ipString, int ipStringLen)
{
	snprintf(ipString, ipStringLen, "%d.%d.%d.%d",
			 ip & 0xFF,
			 (ip & 0xFF00) >> 8,
			 (ip & 0xFF0000) >> 16,
			 (ip & 0xFF000000) >> 24);
}



/* ****************************************************************************
*
* accept - 
*/
Endpoint2* Endpoint2::accept(bool addToEpVec)
{
	int                 fd;
	struct sockaddr_in  sin;
	char                hostName[64];
	unsigned int        len         = sizeof(sin);
	int                 hostNameLen = sizeof(hostName);
	Endpoint2*          ep          = NULL;

	if (type != Listener)
		LM_RE(NULL, ("A non Listener endpoint cannot accept connections ..."));
	
	memset((char*) &sin, 0, len);

	if ((fd = ::accept(rFd, (struct sockaddr*) &sin, &len)) == -1)
		LM_RP(NULL, ("accept"));

	ip2string(sin.sin_addr.s_addr, hostName, hostNameLen);

	Host* hostP;
	hostP = epMgr->hostMgr->insert(NULL, hostName);

	if (addToEpVec == true)
		ep = epMgr->add(Anonymous, NULL, NULL, hostP, 0, fd, fd);

	return ep;
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



/* ****************************************************************************
*
* listenerPrepare - 
*/
Endpoint2::Status Endpoint2::listenerPrepare(void)
{
	int                 reuse = 1;
	struct sockaddr_in  sock;
	struct sockaddr_in  peer;

	if (type != Listener)
		LM_RE(NotListener, ("Cannot prepare an Endpoint that is not a listener ..."));

	if (rFd != -1)
		LM_W(("This listener already seems to be prepared ... Continuing anuway"));

	if ((rFd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
		LM_RP(SocketError, ("socket"));

	fcntl(rFd, F_SETFD, 1);

	memset((char*) &sock, 0, sizeof(sock));
	memset((char*) &peer, 0, sizeof(peer));

	sock.sin_family      = AF_INET;
	sock.sin_addr.s_addr = INADDR_ANY;
	sock.sin_port        = htons(port);
	
	setsockopt(rFd, SOL_SOCKET, SO_REUSEADDR, (char*) &reuse, sizeof(reuse));
	
	if (bind(rFd, (struct sockaddr*) &sock, sizeof(struct sockaddr_in)) == -1)
	{
		close(rFd);
		rFd = -1;
		LM_RP(BindError, ("bind to port %d: %s", port, strerror(errno)));
	}

	if (listen(rFd, 10) == -1)
	{
		close(rFd);
		rFd = -1;
		LM_RP(ListenError, ("listen to port %d", port));
	}

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
	int                  dataLen  = 0;
	Packet               packet;
	Endpoint2::Status    s;
	Message::HelloData*  helloP;

	s = receive(&header, &dataP, &dataLen, &packet);
	if (s != 0)
		LM_RE(s, ("receive error '%s'", status(s)));

	switch (header.code)
	{
	case Message::Hello:
		helloP = (Message::HelloData*) dataP;
		s = helloDataAdd((Type) helloP->type, helloP->name, helloP->alias);
		if (s != OK)
			LM_RE(s, ("Bad hello data"));
		if (header.type == Message::Msg)
			helloSend(Message::Ack);
		break;

	default:
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
	case Anonymous:      return "Anonymous";
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

	return "Unknown Endpoint Type";
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
	case ConnectionClosed:     return "ConnectionClosed";
	case Timeout:              return "Timeout";

	case NullAlias:            return "NullAlias";
	case BadAlias:             return "BadAlias";
	case BadHost:              return "BadHost";
	case Duplicated:           return "Duplicated";

	case Error:                return "Error";
	case ReadError:            return "ReadError";
	case WriteError:           return "WriteError";
	case AcceptError:          return "AcceptError";
	case NotListener:          return "NotListener";
	case SelectError:          return "SelectError";
	case BindError:            return "BindError";
	case ListenError:          return "ListenError";
	case SocketError:          return "SocketError";
	}

	return "Unknown Status";
}

}
