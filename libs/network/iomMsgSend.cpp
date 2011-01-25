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
#include "traceLevels.h"        // LMT_NWRUN, ...

#include "Buffer.h"				// ss::Buffer
#include "MemoryManager.h"      // MemoryManager
#include "Endpoint.h"           // Endpoint
#include "Message.h"            // Message::Header, MessageCode, MessageType, ...
#include "Packet.h"             // Packet
#include "iomMsgSend.h"         // Own interface


#if 0
/* ****************************************************************************
*
* global variables
*/
static struct timeval startTime;
static struct timeval endTime;
static long           bytesSent;
#endif



/* ****************************************************************************
*
* iomWriteOk - 
*/
bool iomWriteOk(int fd)
{
	int             fds;
	fd_set          wFds;
	struct timeval  timeVal;
	
	timeVal.tv_sec  = 0;
	timeVal.tv_usec = 0;

	FD_ZERO(&wFds);
	FD_SET(fd, &wFds);
	
	do
	{
		fds = select(fd + 1, NULL, &wFds, NULL, &timeVal);
	} while ((fds == -1) && (errno == EINTR));

	if ((fds == 1) && (FD_ISSET(fd, &wFds)))
		return true;

	LM_W(("cannot write to fd %d", fd));
	return false;
}



/* ****************************************************************************
*
* iomMsgSend - send a message to an endpoint
*
* WARNING
* This function is used to send log lines between processes, thus it cannot use
* log messages!
*/
int iomMsgSend
(
	int                         fd,
	const char*                 to,
	const char*                 from,
	ss::Message::MessageCode    code,
	ss::Message::MessageType    type,
	void*                       data,       
	int                         dataLen,    
	ss::Packet*                 packetP
)
{
	ss::Message::Header   header;
	struct iovec          ioVec[4];
	int                   vecs = 1;
	ssize_t               s;
	bool                  outHook;

	outHook = lmOutHookInhibit();

	if (fd == -1)
	{
		LM_E(("file descriptor for '%s' is -1 !!!", to));
		lmOutHookRestore(outHook);
		return -1;
	}

	memset(&header, 0, sizeof(header));
	memset(ioVec, 0, sizeof(ioVec));

	header.code        = code;
	header.type        = type;
	header.magic       = 0xFEEDC0DE;

	ioVec[0].iov_base  = &header;
	ioVec[0].iov_len   = sizeof(header);
	
	if ((dataLen != 0) && (data == NULL))
		LM_X(1, ("dataLen: %d but dataP == NULL ...", dataLen));

	if ((dataLen == 0) && (data != NULL))
		LM_X(1, ("dataLen: 0 but dataP != NULL ..."));

	if ((dataLen != 0) && (data != NULL))
	{
		header.dataLen        = dataLen;
		ioVec[vecs].iov_base  = data;
		ioVec[vecs].iov_len   = dataLen;
		
		++vecs;
	}

	if ((packetP != NULL) && (packetP->message.ByteSize() != 0))
	{
		char* outputVec;

		header.gbufLen = packetP->message.ByteSize();

		outputVec = (char*) malloc(header.gbufLen + 2);
		if (outputVec == NULL)
			LM_X(1, ("malloc error: %s", strerror(errno)));

		if (packetP->message.SerializeToArray(outputVec, header.gbufLen) == false)
			LM_X(1, ("SerializeToArray failure"));

		ioVec[vecs].iov_base  = outputVec;
		ioVec[vecs].iov_len   = packetP->message.ByteSize();

		++vecs;
	}

	if ((packetP != NULL) && (packetP->buffer != 0))
	{

		header.kvDataLen      = packetP->buffer->getSize();
		ioVec[vecs].iov_base  = packetP->buffer->getData();
		ioVec[vecs].iov_len   = packetP->buffer->getSize();
		
		++vecs;
	}

	if (iomWriteOk(fd) == false)
	{
		LM_E(("Cannot write to fd %d (returning -2 as if it was a 'connection closed' ...)"));
        lmOutHookRestore(outHook);
		return -2;
	}

	s = writev(fd, ioVec, vecs);
	if ((s == -1) || (s == 0))
	{
		if (s == -1)
			LM_E(("writev: %s", strerror(errno)));
		else
			LM_E(("writev returned 0"));

		lmOutHookRestore(outHook);
		return -1;
	}

	if (s != (ssize_t) (ioVec[0].iov_len + ioVec[1].iov_len + ioVec[2].iov_len + ioVec[3].iov_len))
		LM_X(1, ("writev returned %d instead of %d", s, ioVec[0].iov_len + ioVec[1].iov_len + ioVec[2].iov_len + ioVec[3].iov_len));

	if (packetP != NULL)
	{
		ss::MemoryManager::shared()->destroyBuffer(packetP->buffer);
		delete packetP;
	}

	lmOutHookRestore(outHook);
	return 0;
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
	bool                  outHook;

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

	if ((packetP != NULL) && (packetP->message.ByteSize() != 0))
		header.gbufLen = packetP->message.ByteSize();

	if (packetP && (packetP->buffer != 0))
		header.kvDataLen = packetP->buffer->getSize();

	LM_T(LMT_MSG, ("Sending '%s' %s to '%s', data bytes: { data: %d, gbuf: %d, kv: %d }",
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

	if ((packetP != NULL) && (packetP->message.ByteSize() != 0))
	{
		char* outputVec;

		outputVec = (char*) malloc(header.gbufLen + 2);
		if (outputVec == NULL)
			LM_XP(1, ("malloc(%d)", header.gbufLen));

		if (packetP->message.SerializeToArray(outputVec, header.gbufLen) == false)
			LM_X(1, ("SerializeToArray failed"));

		s = partWrite(to, outputVec, packetP->message.ByteSize(), "Google Protocol Buffer");
		free(outputVec);
		if (s != packetP->message.ByteSize())
		{
			LM_E(("partWrite returned %d and not the expected %d", s, packetP->message.ByteSize()));
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
		ss::MemoryManager::shared()->destroyBuffer(packetP->buffer);
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
