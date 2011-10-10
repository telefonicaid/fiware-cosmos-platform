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

#include "logMsg/logMsg.h"
#include "logMsg/traceLevels.h"

#include "au/Token.h"
#include "au/TokenTaker.h"                          // au::TokenTake
#include "au/list.h"

#include "engine/DiskStatistics.h"          // samson::DiskStatistics

#include "samson/common/status.h"
#include "samson/common/MemoryTags.h"         // MemoryOutputDisk
#include "samson/common/ports.h"              // WORKER_PORT, ...

#include "Packet.h"
#include "HostMgr.h"
#include "Message.h"
#include "EndpointManager.h"
#include "Endpoint2.h"



namespace samson
{



/* ****************************************************************************
*
* readerThread - 
*/
void* readerThread(void* vP)
{
	Endpoint2* ep = (Endpoint2*) vP;

	LM_T(LmtThreads, ("Reader thread 0x%x for endpoint %s is running", pthread_self(), ep->name()));
	ep->run();

	ep->stateSet(Endpoint2::ScheduledForRemoval);
	LM_T(LmtThreads, ("Endpoint %s is back from Endpoint2::run. Leaving reader thread 0x%x", ep->name(), pthread_self()));
	ep->readerId = 0;
	return NULL;
}



/* ****************************************************************************
*
* writerThread - 
*/
void* writerThread(void* vP)
{
	Endpoint2* ep = (Endpoint2*) vP;

	LM_T(LmtThreads, ("Writer thread(%s) 0x%x is running (REAL wFd: %d))    Endpoint at %p", ep->name(), pthread_self(), ep->wFdGet(), ep));

	while (1)
	{
		Packet* packetP;

        {
            // Protect the access with a TokenTaker
            au::TokenTaker tk( &ep->jobQueueSem );
            
            packetP = ep->jobQueue.extractFront();
        }
        
		LM_T(LmtThreads, ("Writer thread(%s) packetP: %p, jobs remaining: %d, my thread id: 0x%x", ep->name(), packetP, ep->jobQueue.size(), pthread_self()));
		if (packetP != NULL)
		{
			Status s;

			LM_T(LmtThreads, ("Writer thread(%s) Got a job from job queue - forwarding", ep->name()));
			s = ep->realsend(packetP->msgType, packetP->msgCode, packetP->dataP, packetP->dataLen, packetP);
			if (s != OK)
			{
				LM_E(("realsend(%s): %s", ep->name(), status(s)));
				LM_T(LmtThreads, ("Leaving writer thread(%s) 0x%x because of send error", ep->name(), ep->writerId));
				ep->stateSet(Endpoint2::ScheduledForRemoval);
				ep->writerId = 0;
				return NULL;
			}
		}
		else
        {
            au::TokenTaker tk( &ep->jobQueueSem );
            tk.stop(-1);// wait forever
        }
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
	Host*             _host,
	unsigned short    _port,
	int               _rFd,
	int               _wFd
 ) : jobQueueSem("Endpoint2")
{
	epMgr               = _epMgr;
	type                = _type;
	id                  = _id;
	host                = _host;
	rFd                 = _rFd;
	wFd                 = _wFd;
	port                = _port;
	state               = Unused;
	threaded            = false;
	nameidhost          = NULL;
	idInEndpointVector  = -8;    // -8 meaning 'undefined' ...
	readerId            = 0;
	writerId            = 0;
	//jobQueueSem         = NULL; // Andreu: Not a pointer anymore

	nameSet(type, id, host);
	memset(&sockin, 0, sizeof(sockin));

	msgsIn        = 0;
	msgsOut       = 0;
	bytesIn       = 0;
	bytesOut      = 0;
	msgsInErrors  = 0;
	msgsOutErrors = 0;
}



/* ****************************************************************************
*
* Endpoint2::~Endpoint2 - 
*/
Endpoint2::~Endpoint2()
{
	close();

	if (nameidhost != NULL)
		free(nameidhost);

    // Andreu: Not a pointer anymore
    /*
	if (jobQueueSem != NULL)
		delete jobQueueSem;
     */

	nameidhost = NULL;
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



/* ****************************************************************************
*
* name - 
*/
const char* Endpoint2::name(void)
{
	return nameidhost;
}



/* ****************************************************************************
*
* nameSet - 
*/
void Endpoint2::nameSet(Type _type, int id, Host* host)
{
	char myname[128];

	if (nameidhost != NULL)
		free(nameidhost);
	
	if ((type == Worker) || (type == Delilah))
		snprintf(myname, sizeof(myname), "%s%02d@%s", typeName(), id, host->name);
	else
		snprintf(myname, sizeof(myname), "%s@%s", typeName(), host->name);

	nameidhost = strdup(myname);
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
	case Loopback:                  return "Loopback";
	case Connected:                 return "Connected";
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
Status Endpoint2::okToSend(void)
{
	int             fds;
	fd_set          wFds;
	struct timeval  timeVal;
	int             tryh;
	int             tries = 3000;

	for (tryh = 0; tryh < tries; tryh++)
	{
		timeVal.tv_sec  = 0;
		timeVal.tv_usec = 100000;

		FD_ZERO(&wFds);
		FD_SET(wFd, &wFds);
	
		do
		{
			fds = select(wFd + 1, NULL, &wFds, NULL, &timeVal);
		} while ((fds == -1) && (errno == EINTR));

		if ((fds == 1) && (FD_ISSET(wFd, &wFds)))
			return OK;

        if( fds == -1 )
            LM_RE(Error, ("Select: %s", strerror(errno)));
        
        if( tryh%10 == 0 )
            LM_W(("Problems to send to %s (%d/%d secs)", name(), tryh/10, tries/10));
	}

	return Timeout;
}



/* ****************************************************************************
*
* partWrite - 
*/
Status Endpoint2::partWrite(void* dataP, int dataLen, const char* what)
{
	int    nb;
	int    tot  = 0;
	char*  data = (char*) dataP;
	Status s;

	while (tot < dataLen)
	{
		if ((s = okToSend()) != OK)
			LM_RE(s, ("Cannot write to '%s' (fd %d) (returning -2 as if it was a 'connection closed' ...)", name(), wFd));

		nb = write(wFd, &data[tot], dataLen - tot);
		if (nb == -1)
			LM_RE(WriteError, ("error writing to '%s' (fd: %d): %s", name(), wFd, strerror(errno)));
		else if (nb == 0)
			LM_RE(WriteError, ("part-write written ZERO bytes to '%s' (total: %d)", name(), tot));

		tot += nb;
	}

	LM_WRITES(name(), what, dataP, dataLen, LmfByte);

	if (epMgr->networkInterface != NULL)
		epMgr->networkInterface->statistics->item_write.add( tot );
	else
		LM_W(("No WRITE statistics as networkInterface is NULL"));

	bytesOut += tot;
	return OK;
}



/* ****************************************************************************
*
* ack - 
*/
void Endpoint2::ack(Message::MessageCode code, void* data, int dataLen)
{
	Packet* packet = new Packet(Message::Ack, code, data, dataLen);
	packet->fromId = idInEndpointVector;
	send(packet);
}



/* ****************************************************************************
*
* send - 
*/
void Endpoint2::send(Packet* packetP)
{
	packetP->fromId = idInEndpointVector;

	if ((host == epMgr->me->host) && (type == epMgr->me->typeGet()))
	{
		LM_T(LmtMsg, ("Loopback: %s %s", Message::messageCode(packetP->msgCode), Message::messageType(packetP->msgType)));

		if (state != Loopback)
			LM_X(1, ("Something went wrong - should be in loopback state ..."));

		if (epMgr->packetReceiver == NULL)
		{
			// LM_X(1, ("No packetReceiver (SW bug) - got a message/ack from %s", name()));
			LM_W(("%s %s from %s: no packet receiver, throwing away the packet",  Message::messageCode(packetP->msgCode), Message::messageType(packetP->msgType), name()));
		}
		else
			epMgr->packetReceiver->_receive(packetP);
	}
	else
	{
		if (threaded == true)
		{
			if ((state != Ready) && (packetP->disposable == true))
			{
				LM_W(("Throwing away disposable packet meant for %s", name()));
				delete packetP;
			}
			else
			{
                au::TokenTaker tk( &jobQueueSem );
				jobQueue.push_back(packetP);
                
                tk.wakeUp();
			}
		}
		else
		{
			if ((state != Ready) && (state != Connected))
				LM_E(("Cannot send '%s' packet to %s - endpoint is in state '%s' - throwing away the packet", messageCode(packetP->msgCode), name(), stateName()));
			else
				realsend(packetP->msgType, packetP->msgCode, packetP->dataP, packetP->dataLen, packetP);
		}
	}
}



/* ****************************************************************************
*
* realsend - 
*/
Status Endpoint2::realsend
(
	Message::MessageType  msgType,
	Message::MessageCode  msgCode,
	void*                 data,
	int                   dataLen,
	Packet*               packetP
)
{
	Status           s;
	Message::Header  header;

	//
	// Sanity check
	//
	if ((msgType != Message::Msg) && (msgType != Message::Evt) && (msgType != Message::Ack) && (msgType != Message::Nak))
		LM_X(1, ("Bad message type: 0x%x", msgType));

	if (msgCode == Message::Die)
		LM_W(("Sending a Die '%s' to %s", messageType(msgType), name()));
	else
		LM_T(LmtSend, ("Sending a '%s' '%s' to %s", messageCode(msgCode), messageType(msgType), name()));
		


	//
	// Preparing header
	//
	memset(&header, 0, sizeof(header));

	header.code        = msgCode;
	header.type        = msgType;
	header.magic       = 0xFEEDC0DE;

	if ((dataLen != 0) && (data != NULL))
		header.dataLen = dataLen;

	if ((packetP != NULL) && (packetP->message != NULL) && (packetP->message->ByteSize() != 0))
		header.gbufLen = packetP->message->ByteSize();

	if (packetP && (packetP->buffer != 0))
		header.kvDataLen = packetP->buffer->getSize();



	//
	// Sending header
	//
	s = partWrite(&header, sizeof(header), "header");
	if (s != OK)
	{
		if (packetP->buffer != NULL)
			engine::MemoryManager::shared()->destroyBuffer(packetP->buffer);
		if (packetP != NULL)
			delete packetP;
		LM_RE(s, ("partWrite:header(%s): %s", name(), status(s)));
	}

	LM_T(LmtMsg, ("Sent a '%s' (0x%x, 0x%x) %s to %s (thread 0x%x)", messageCode(header.code), header.code, msgCode, messageType(header.type), name(), pthread_self()));



	//
	// Sending raw data
	//
	if ((dataLen != 0) && (data != NULL))
	{
		s = partWrite(data, dataLen, "msg data");
		if (s != OK)
		{
#if 0
			if (packetP->buffer != NULL)
				engine::MemoryManager::shared()->destroyBuffer(packetP->buffer);
			if (packetP != NULL)
				delete packetP;
            
#endif
			LM_RE(s, ("partWrite:data(%s): %s", name(), status(s)));
		}
	}



	//
	// Sending Google Protocol Buffer
	//
	if ((packetP != NULL) && (packetP->message != NULL) && (packetP->message->ByteSize() != 0))
	{
		char* outputVec;

		outputVec = (char*) malloc(header.gbufLen + 2);
		if (outputVec == NULL)
			LM_XP(1, ("malloc(%d)", header.gbufLen));

		if (packetP->message->SerializeToArray(outputVec, header.gbufLen) == false)
			LM_X(1, ("SerializeToArray failed"));

		s = partWrite(outputVec, packetP->message->ByteSize(), "Google Protocol Buffer");
		free(outputVec);
		if (s != OK)
		{
#if 0
			if (packetP->buffer != NULL)
				engine::MemoryManager::shared()->destroyBuffer(packetP->buffer);
            if (packetP != NULL)
				delete packetP;

#endif
			LM_RE(s, ("partWrite:GoogleProtocolBuffer(): %s", status(s)));
		}
	}

	if (packetP && (packetP->buffer != 0))
	{
		s = partWrite(packetP->buffer->getData(), packetP->buffer->getSize(), "KV data");
		if (s != OK)
		{
#if 0
			if (packetP->buffer != NULL)
				engine::MemoryManager::shared()->destroyBuffer(packetP->buffer);
			if (packetP != NULL)
				delete packetP;
#endif
			LM_RE(s, ("partWrite returned %d and not the expected %d", s, packetP->buffer->getSize()));
		}
	}

	if (packetP != NULL)
	{
		if (packetP->buffer != NULL)
			engine::MemoryManager::shared()->destroyBuffer(packetP->buffer);
		delete packetP;
	}
	
    
	msgsOut += 1;
	return OK;
}



/* ****************************************************************************
*
* partRead - 
*/
Status Endpoint2::partRead(void* vbuf, long bufLen, long* bufLenP, const char* what)
{
	ssize_t  tot = 0;
	Status   s;
	char*    buf = (char*) vbuf;

	while (tot < bufLen)
	{
		ssize_t nb;

        // Andreu: This produced an error when system was heavily loaded ( select in msgAwait return timeout )
        int try_msgAwait = 0;
        do
        {
            s = msgAwait(0, 500000, what);
            if( s!= OK )
            {
                LM_W(("msgAwait: %s, expecting '%s' from %s (%d/%d)", status(s), what, name() , try_msgAwait , 1000 ));
                usleep(500000);
                if( try_msgAwait++ == 1000 )
                    break;
            }
        } while ( s!= OK );
        
		if (s != OK)
			LM_RE(s, ("msgAwait: %s, expecting '%s' from %s", status(s), what, name()));

		nb = read(rFd, (void*) &buf[tot] , bufLen - tot);
		if (nb == -1)
		{
			if ((type == Controller) && (epMgr->me->type == Delilah))
				LM_X(1, ("Controller Dies - I cannot continue. Sorry ..."));

			if (errno == EBADF)
				LM_RE(ConnectionClosed, ("read: %s (treating as Connection Closed), expecting '%s' from %s", strerror(errno), what, name()));

			LM_RE(ConnectionClosed, ("read: %s, expecting '%s' from %s", strerror(errno), what, name()));
		}
		else if (nb == 0)
		{
			if ((type == Controller) && (epMgr->me->type == Delilah))
				LM_X(1, ("Controller Dies - I cannot continue. Sorry ..."));

			LM_RE(ConnectionClosed, ("Connection closed, expecting '%s' from %s. Read 0 bytes from rFd:%d", what, name(), rFd));
		}

		tot += nb;
	}

	if (bufLenP == NULL)
		LM_X(1, ("Got called with NULL buffer length pointer. This is a programmer's bug and must be fixed. Right now. (Expecting '%s' from %s)", what, name()));

	*bufLenP = tot;
	LM_READS(name(), what, buf, tot, LmfByte);

	if (epMgr->networkInterface != NULL)
		epMgr->networkInterface->statistics->item_read.add( tot );
	else
		LM_W(("No READ statistics as networkInterface is NULL"));
    
	bytesIn += tot;
	return OK;
}



/* ****************************************************************************
*
* receive - 
*/
Status Endpoint2::receive(Message::Header* headerP, void** dataPP, long* dataLenP, Packet* packetP)
{
	Status s;
	long   bufLen;
	long   totalBytesReadExceptHeader = 0;

	if ((type == Listener) || (type == WebListener))
		LM_X(1, ("Listener endpoint - should never get here ..."));

	*dataPP = NULL;

	s = partRead(headerP, sizeof(Message::Header), &bufLen, "Header");
	if (s != OK)
	{
		close();
		return s;
	}

	LM_T(LmtMsg, ("Read a '%s' %s from %s", messageCode(headerP->code), messageType(headerP->type), name()));

	if (headerP->dataLen != 0)
	{
		*dataPP = calloc(1, headerP->dataLen);

		s = partRead(*dataPP, headerP->dataLen, &bufLen, "Binary Data");
		if (s != OK)
		{
			close();
			free(*dataPP);

			return s;
		}
		LM_T(LmtReceive, ("Read %d bytes of RAW DATA from %s", bufLen, name()));
		totalBytesReadExceptHeader += bufLen;
	}

	if (headerP->gbufLen != 0)
	{
		char* dataP = (char*) calloc(1, headerP->gbufLen + 1);

		s = partRead(dataP, headerP->gbufLen, &bufLen, "Google Protocol Buffer");
		if (s != OK)
		{
			close();
			free(dataP);
			if (*dataPP != NULL)
				free(*dataPP);

			return s;
		}
		LM_T(LmtReceive, ("Read %d bytes of GOOGLE DATA from '%s'", bufLen, name()));

		packetP->message->ParseFromArray(dataP, headerP->gbufLen);
		if (packetP->message->IsInitialized() == false)
			LM_X(1, ("Error parsing Google Protocol Buffer of %d bytes because a message %s is not initialized!",
					 headerP->gbufLen, samson::Message::messageCode(headerP->code)));
		free(dataP);
		totalBytesReadExceptHeader += bufLen;
	}

	if (headerP->kvDataLen != 0)
	{
		char         kvName[128];
		static int   bIx = 0;

		sprintf(kvName, "%s:%d", name(), bIx);
		++bIx;

		packetP->buffer = engine::MemoryManager::shared()->newBuffer(kvName, headerP->kvDataLen, samson::MemoryOutputDisk  );

		char*  kvBuf  = packetP->buffer->getData();
		long   nb     = 0;

		s = partRead(kvBuf, headerP->kvDataLen, &nb, "Key-Value Data");
		if (s != OK)
		{
			close();
			if (*dataPP != NULL)
				free(*dataPP);

			return s;
		}
		LM_T(LmtReceive, ("Read %d bytes of KV DATA from '%s'", nb, name()));

		packetP->buffer->setSize(nb);
		totalBytesReadExceptHeader += bufLen;
	}

	if (dataLenP == NULL)
		LM_X(1, ("Got called with NULL buffer length pointer. This is a programmer's bug and must be fixed. Right now. Come on, do it!"));
	*dataLenP = totalBytesReadExceptHeader;

	msgsIn += 1;
	return OK;
}



/* ****************************************************************************
*
* connect - 
*/
Status Endpoint2::connect(void)
{
	struct hostent*     hp;
	struct sockaddr_in  peer;

	if (host == NULL)
		LM_RE(NullHost, ("Cannot connect to endpoint '%s' with NULL host!", name()));
	if (port == 0)
		LM_RE(NullPort, ("Cannot connect to '%s' - port is ZERO", name()));

	if ((hp = gethostbyname(host->name)) == NULL)
		LM_RE(GetHostByNameError, ("gethostbyname(%s): %s", host->name, strerror(errno)));

	if ((rFd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
		LM_RE(SocketError, ("socket: %s", strerror(errno)));
	
	wFd = rFd;

	memset((char*) &peer, 0, sizeof(peer));

	peer.sin_family      = AF_INET;
	peer.sin_addr.s_addr = ((struct in_addr*) (hp->h_addr))->s_addr;
	peer.sin_port        = htons(port);

	LM_T(LmtConnect, ("Connecting to %s at %s:%d", name(), host->name, port));
	if (::connect(wFd, (struct sockaddr*) &peer, sizeof(peer)) == -1)
	{
		usleep(50000);
		LM_W(("connect: %s", strerror(errno)));
		if (::connect(wFd, (struct sockaddr*) &peer, sizeof(peer)) == -1)
		{
			close();
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
	state = Connected;

	return OK;
}



/* ****************************************************************************
*
* msgAwait - 
*/
Status Endpoint2::msgAwait(int secs, int usecs, const char* what)
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
		LM_RP(SelectError, ("select error awaiting '%s' from '%s", what, name()));
	else if (fds == 0)
		LM_RE(Timeout, ("timeout awaiting '%s' from '%s' (%d.%06d seconds)", what, name(), secs, usecs));
	else if ((fds > 0) && (!FD_ISSET(rFd, &rFds)))
		LM_RE(Error, ("some other fd has a read pending - this is impossible ! (awaiting '%s' from '%s')", what, name()));
	else if ((fds > 0) && (FD_ISSET(rFd, &rFds)))
		return OK;

	LM_X(1, ("Very strange error awaiting '%s' from '%s'", what, name()));

	return Error;
}



/* ****************************************************************************
*
* msgTreat - 
*/
Status Endpoint2::msgTreat(void)
{
	Message::Header      header;
	void*                dataP    = NULL;
	long                 dataLen  = 0;
	Packet*              packetP;
	Status               s;
	Message::HelloData*  helloP;

	if ((type == Listener) || (type == WebListener) || (type == WebWorker))
	{
		LM_T(LmtMsgTreat, ("Something to read for %s - calling msgTreat2", name()));
		return msgTreat2();
	}
	
	LM_T(LmtMsgTreat, ("Reading incoming message from '%s'", name()));
	s = msgAwait(-1, -1, "Incoming message");
	if (s != OK)
		LM_E(("msgAwait: %s - what do I do ... ?", status(s)));

	packetP = new Packet(Message::Unknown);
	s = receive(&header, &dataP, &dataLen, packetP);
	if (s != 0)
	{
		delete packetP;

		if (s == ConnectionClosed)
		{
			state = ScheduledForRemoval;
			epMgr->show("ConnectionClosed", true);
			return s;
		}

		LM_TODO(("I should probably remove the endpoint also if error is NOT Connection Closed"));
		LM_RE(s, ("receive(%s): '%s'", name(), status(s)));
	}

	packetP->msgCode = header.code;
	packetP->msgType = header.type;
	packetP->fromId  = idInEndpointVector;
	packetP->dataLen = dataLen;
	packetP->dataP   = dataP;
	if (type == Unhelloed)
		return msgTreat2(packetP);

	switch (header.code)
	{
	case Message::Hello:
		helloP = (Message::HelloData*) dataP;
		LM_T(LmtHello, ("Got a Hello %s from %s", messageType(header.type), name()));
		helloDataSet((Type) helloP->type, helloP->id);

		if (header.type == Message::Msg)
			helloSend(Message::Ack);

		if ((epMgr->me->type == Spawner) || (epMgr->me->type == Setup) || (epMgr->me->type == Controller))
		{
			LM_T(LmtThreads, ("I'm a Spawner/Controller/Setup - I don't use sender/reader threads!"));
			delete packetP;
			return OK;
		}

		LM_T(LmtHello, ("Received Hello from %s (%d jobs pending)", name(), jobQueue.size()));
		if (((epMgr->me->type == Worker) || (epMgr->me->type == Delilah)) && ((type == Worker) || (type == Delilah) || (type == Controller)))
		{
			int  ps;
			char semName[128];

			threaded    = true;
			snprintf(semName, sizeof(semName), "jobQueue-%s", name());

			//jobQueueSem     = new au::Token(semName);
            // Andreu: Not a pointer anymore

			LM_T(LmtThreads, ("Creating writer and reader threads for endpoint %s (plus jobQueueSem and jobQueueStopper)", name()));

			if ((ps = pthread_create(&writerId, NULL, writerThread, this)) != 0)
			{
				LM_E(("Creating writer thread: pthread_create returned %d for %s", ps, name()));
				delete packetP;
				return PThreadError;
			}
			LM_T(LmtThreads, ("Created Writer Thread with id 0x%x", writerId));

			if ((ps = pthread_create(&readerId, NULL, readerThread, this)) != 0)
			{
				LM_E(("Creating reader thread: pthread_create returned %d for %s", ps, name()));
				delete packetP;
				return PThreadError;
			}
			LM_T(LmtThreads, ("Created Reader Thread with id 0x%x", readerId));

			
		}

		epMgr->show("Received Hello", true);
		break;

	case Message::Ping:
		if (header.type == Message::Msg)
		{
			LM_F(("Got a ping from %s - responding", name()));
			ack(Message::Ping, packetP->dataP, packetP->dataLen);
		}
		else
			LM_F(("Got a ping ACK from %s", name()));
			
		break;

	default:
		LM_T(LmtMsgTreat, ("Don't know how to treat '%s' %s (code %d), passing it to msgTreat2", messageCode(header.code), messageType(header.type), header.code));
		s = msgTreat2(packetP);
		if (s != OK)
		{
			delete packetP;
			LM_RE(s, ("msgTreat2: %s", status(s)));
		}
		return OK; // To avoid to delete packetP ...
		break;
	}

	delete packetP;
	return OK;
}



/* ****************************************************************************
*
* run - 
*/
void Endpoint2::run(void)
{
	LM_T(LmtThreads, ("Endpoint '%s' reader thread is running", name()));

	while (1)
	{
		Status s;

		LM_T(LmtThreads, ("Reader Thread for '%s' awaiting a packet", name()));
		s = msgTreat();
		if (s == ConnectionClosed)
		{
			LM_W(("Endpoint %s closed connection", name()));
			return;
		}
	}
}



/* ****************************************************************************
*
* die - send die to endpoint and await death, with timeout
*/
Status Endpoint2::die(int secs, int usecs)
{
	char    c;
	int     nb;
	Status  s;
	Packet* packetP = new Packet(Message::Msg, Message::Die);

	packetP->fromId = idInEndpointVector;
	send(packetP);

	if ((s = msgAwait(secs, usecs, "Connection Closed")) != OK)
		LM_RE(s, ("msgAwait: %s", status(s)));

	nb = read(rFd, &c, 1);
	if (nb == -1)
		LM_RE(ReadError, ("Expected to read 0 bytes, meaning 'Connection Closed', but read() returned -1: %s", strerror(errno)));
	else if (nb != 0)
		LM_RE(KillError, ("Endpoint not dead, even though a Die message was sent!"));

	LM_TODO(("This endpoint must be removed from EndpointManager, right?"));
	close();

	return OK;
}



/* ****************************************************************************
*
* - 
*/
bool Endpoint2::isThreaded(void)
{
	return threaded;
}



/* ****************************************************************************
*
* helloDataSet - 
*/
Status Endpoint2::helloDataSet(Type _type, int _id)
{
	if (type != Unhelloed)
	{
		if (type != _type)
			LM_W(("Got a Hello from %s, saying he's of '%s' type, when endpoint says '%s'", name(), typeName(_type), typeName()));
	}

	type  = _type;
	id    = _id;
	state = Ready;

	nameSet(type, id, host);
	return OK;
}



/* ****************************************************************************
*
* helloSend - 
*/
void Endpoint2::helloSend(Message::MessageType msgType)
{
	Message::HelloData  hello;
	Status              s;

	LM_T(LmtHello, ("Sending Hello to %s", name()));

	memset(&hello, 0, sizeof(hello));

	strncpy(hello.ip, epMgr->me->hostname(), sizeof(hello.ip));

	hello.type     = epMgr->me->typeGet();
	hello.coreNo   = 0;
	hello.id       = epMgr->me->id;

	LM_T(LmtHello, ("sending hello %s to '%s' (my type: '%s', id: %d)", messageType(msgType), name(), epMgr->me->typeName(), epMgr->me->id));

	if ((s = realsend(msgType, Message::Hello, &hello, sizeof(hello))) != OK)
		LM_E(("Error sending Hello to %s: %s", name(), status(s)));
}



/* ****************************************************************************
*
* close - 
*/
void Endpoint2::close(void)
{
	if (state != ScheduledForRemoval)
		state = Disconnected;

	if (type == Unhelloed)
		state = ScheduledForRemoval;

	if (rFd != -1)
		::close(rFd);
	if ((wFd != rFd) && (wFd != -1))
		::close(wFd);

	rFd  = -1;
	wFd  = -1;
}



/* ****************************************************************************
*
* statusString - 
*/
const char* Endpoint2::statusString(char* buf, int bufLen, int ix)
{
	char       jq[32];
	char       fd[16];
	char       conSign;

	if (threaded == true)
		snprintf(jq, sizeof(jq), "(qjobs: %d)", (int) jobQueue.size());
	else
		jq[0] = 0;


	if (rFdGet() == -1)
		fd[0] = 0;
	else
		snprintf(fd, sizeof(fd), "%02d", rFdGet());
	

	if ((state == Connected || state == Ready || state == Loopback) && (rFd >= 0))
		conSign = '+';
	else
		conSign = '-';
	



	snprintf(buf, bufLen, "%c%c%02d: %-12s %02d  %-20s %-20s %04d  %-2s (in: %03d/%s, out: %03d/%s) %s\n",
			 conSign,
			 (threaded == true)? 's' : ' ',
			 ix,
			 typeName(),
			 idGet(),
			 hostname(),
			 stateName(),
			 port,
			 fd,
			 msgsIn,
			 au::str(bytesIn, "B").c_str(),
			 msgsOut,
			 au::str(bytesOut, "B").c_str(),
			 jq);

	return buf;
}

}
