/* ****************************************************************************
*
* FILE                     iomMsgSend.cpp
*
* AUTHOR                   Ken Zangelin
*
* CREATION DATE            Oct 13 2010
*
*/
#include <unistd.h>             // write
#include <memory.h>             // memset
#include <sys/uio.h>            // writev, struct iovec, ...
#include <sys/time.h>           // gettimeofday

#include "logMsg.h"             // LM_*
#include "traceLevels.h"        // LmtWrite, ...

#include "Buffer.h"				// ss::Buffer
#include "MemoryManager.h"      // MemoryManager
#include "Endpoint.h"           // Endpoint
#include "Message.h"            // Message::Header, MessageCode, MessageType, ...
#include "Packet.h"             // Packet
#include "iomMsgSend.h"         // Own interface


/* ****************************************************************************
*
* iomWriteOk - 
*/
bool iomWriteOk(int fd, const char* name, const char* ip)
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
		FD_SET(fd, &wFds);
	
		do
		{
			fds = select(fd + 1, NULL, &wFds, NULL, &timeVal);
		} while ((fds == -1) && (errno == EINTR));

		if ((fds == 1) && (FD_ISSET(fd, &wFds)))
			return true;

		LM_W(("Problems to send to %s@%s (%d/%d secs)", name, ip, tryh, tries));
	}

	LM_X(1, ("cannot write to fd %d", fd));
	return false;
}



/* ****************************************************************************
*
* partWrite - 
*/
static int partWrite(ss::Endpoint* to, void* dataP, int dataLen, const char* what)
{
	int    nb;
	int    tot  = 0;
	char*  data = (char*) dataP;

	while (tot < dataLen)
	{
		if (iomWriteOk(to->wFd, to->name.c_str(), to->ip) == false)
		{
			LM_E(("Cannot write to '%s' (fd %d) (returning -2 as if it was a 'connection closed' ...)", to->name.c_str(), to->wFd));
			return -2;
		}
		
		nb = write(to->wFd, &data[tot], dataLen - tot);
		if (nb == -1)
		{
			to->msgsOutErrors += 1;
			LM_RE(-1, ("error writing to '%s': %s", to->name.c_str(), strerror(errno)));
		}
		else if (nb == 0)
		{
			to->msgsOutErrors += 1;
			LM_RE(-1, ("part-write written ZERO bytes to '%s' (total: %d)", to->name.c_str(), tot));
		}

		tot += nb;
	}

	LM_WRITES(to->name.c_str(), what, data, dataLen, LmfByte);

	return tot;
}



/* ****************************************************************************
*
* iomMsgSend - send a message to an endpoint
*/
int iomMsgSend
(
	ss::Endpoint*               to,
	ss::Endpoint*               from,
	ss::Message::MessageCode    code,
	ss::Message::MessageType    type,
	void*                       data,       
	int                         dataLen,    
	ss::Packet*                 packetP
)
{
	ss::Message::Header  header;
	int                  s;
	struct timeval       start;
	struct timeval       end;
	bool                 outHook;

	outHook = lmOutHookInhibit();

	if (code == ss::Message::Die)
		LM_W(("%s sending a Die to %s", from->name.c_str(), to->name.c_str()));

	gettimeofday(&start, NULL);

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

	LM_T(LmtWrite, ("Sending '%s' %s to '%s', data bytes: { data: %d, gbuf: %d, kv: %d }",
				   messageCode(code), messageType(type), to->name.c_str(), header.dataLen, header.gbufLen, header.kvDataLen));

	s = partWrite(to, &header, sizeof(header), "header");
	if (s != sizeof(header))
	{
		LM_E(("partWrite returned %d and not the expected %d", s, sizeof(header)));
		lmOutHookRestore(outHook);
		return -1;
	}

	if ((dataLen != 0) && (data != NULL))
	{
		s = partWrite(to, data, dataLen, "msg data");
		if (s != dataLen)
		{
			LM_E(("partWrite returned %d and not the expected %d", s, dataLen));
			lmOutHookRestore(outHook);
			return -1;
		}
	}

	if ((packetP != NULL) && (packetP->message->ByteSize() != 0))
	{
		char* outputVec;

		outputVec = (char*) malloc(header.gbufLen + 2);
		if (outputVec == NULL)
			LM_XP(1, ("malloc(%d)", header.gbufLen));

		if (packetP->message->SerializeToArray(outputVec, header.gbufLen) == false)
			LM_X(1, ("SerializeToArray failed"));

		s = partWrite(to, outputVec, packetP->message->ByteSize(), "Google Protocol Buffer");
		free(outputVec);
		if (s != packetP->message->ByteSize())
		{
			LM_E(("partWrite returned %d and not the expected %d", s, packetP->message->ByteSize()));
			lmOutHookRestore(outHook);
			return -1;
		}
	}

	if (packetP && (packetP->buffer != 0))
	{
		s = partWrite(to, packetP->buffer->getData(), packetP->buffer->getSize(), "KV data");
		if (s != (int) packetP->buffer->getSize())
		{
			LM_E(("partWrite returned %d and not the expected %d", s, packetP->buffer->getSize()));
			lmOutHookRestore(outHook);
			return -1;
		}
	}

	if (packetP != NULL)
	{
		ss::Engine::shared()->memoryManager.destroyBuffer(packetP->buffer);
		delete packetP;
	}

	int bytesSent = sizeof(header) + header.dataLen + header.gbufLen + header.kvDataLen;
	to->msgsOut  += 1;
	to->bytesOut += bytesSent;

	if (bytesSent > 100)
	{
		struct timeval diff;
		int            usecs;

		gettimeofday(&end, NULL);
		diff.tv_sec  = end.tv_sec  - start.tv_sec;
		diff.tv_usec = end.tv_usec - start.tv_usec;

		if (diff.tv_usec < 0)
		{
			diff.tv_sec  -= 1;
			diff.tv_usec += 1000000;
		}

		usecs = diff.tv_sec * 1000000 + diff.tv_usec;

		if (usecs < 0)
			LM_X(1, ("usecs cannot be < 0 ..."));

		if (usecs == 0)
		{
			lmOutHookRestore(outHook);
			return 0;
		}

		to->wMbps = bytesSent / usecs;

		to->wAccMbps = (to->wAccMbps * to->writes + to->wMbps) / (to->writes + 1);
		to->writes += 1;
	}

	lmOutHookRestore(outHook);
	return 0;
}	
