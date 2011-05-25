/* ****************************************************************************
*
* FILE                     iomMsgRead.cpp
*
* AUTHOR                   Ken Zangelin
*
* CREATION DATE            Oct 13 2010
*
*/
#include <unistd.h>             // write
#include <memory.h>             // memset
#include <sys/time.h>           // gettimeofday

#include "logMsg/logMsg.h"             // LM_*
#include "logMsg/traceLevels.h"        // LmtRead, ...

#include "samson/network/Message.h"            // MessageType, Code, etc.
#include "samson/network/iomMsgAwait.h"        // iomMsgAwait
#include "samson/network/Endpoint.h"           // Endpoint
#include "samson/network/Packet.h"             // Packet
#include "engine/Buffer.h"             // Buffer
#include "engine/MemoryManager.h"      // MemoryManager
#include "samson/network/iomMsgRead.h"                 // Own interface
#include "logMsg/logMsg.h"                     // Logs
#include "samson/common/MemoryTags.h"                 // MemoryOutputDisk


/* ****************************************************************************
*
* iomMsgPartRead - read from a socket until completed or error
*/
static ssize_t iomMsgPartRead(const char* from, const char* what, int fd, char* buf, ssize_t bufLen)
{
	ssize_t  tot = 0;
	int      s;

	while (tot < bufLen)
	{
		ssize_t nb;


		int trials = 0;
		do
		{
		   s = iomMsgAwait(fd, 0, 500000);
		   if ( s!=1 )
		   {
			  sleep(1);
			  trials++;

			  if( trials > 5 )
				 LM_W(("iomMsgAwait returned %d. Trial %d. Sleeping and trying again...",s,trials));
			  else
				 LM_D(("iomMsgAwait returned %d. Trial %d. Sleeping and trying again...",s,trials));			  
			  
			  if( trials >= 50 )
				 break;
		   }
		} while( s!= 1 );


		if (s != 1)
			LM_RE(-1, ("iomMsgAwait(%s) returned %d", from, s));

		nb = read(fd, (char*) buf + tot , bufLen - tot);
		if (nb == -1)
		{
			if (errno == EBADF)
				LM_RE(-2, ("read(%s): %s (treating as Connection Closed)", from, strerror(errno)));

			LM_RE(-2, ("read(%s): %s", from, strerror(errno)));
		}
		else if (nb == 0)
		{
			if (tot != 0)
			{
				LM_T(LmtRead, ("read %d bytes %s from %s (fd %d)", tot, what, from, fd));
				LM_READS(from, what, buf, tot, LmfByte);
			}
			else
				LM_T(LmtRead, ("Connection Closed by %s", from));

			return -2;
		}

		tot += nb;
	}

	LM_T(LmtRead, ("read %d bytes from %s (fd %d)", tot, from, fd));
	LM_READS(from, what, buf, tot, LmfByte);

	return tot;
}



/* ****************************************************************************
*
* iomMsgPartRead - 
*/
ssize_t iomMsgPartRead(samson::Endpoint* ep, const char* what, char* buf, ssize_t bufLen)
{
	return iomMsgPartRead(ep->name.c_str(), what, ep->rFd, buf, bufLen);
}



/* ****************************************************************************
*
* iomMsgRead - read a message from an endpoint
*/
int iomMsgRead
(
	samson::Endpoint*              ep,
	samson::Message::Header*       headerP,
	samson::Message::MessageCode*  msgCodeP,
	samson::Message::MessageType*  msgTypeP,
	void**                     dataPP,
	int*                       dataLenP,
	samson::Packet*                packetP,
	void*                      kvData,
	int*                       kvDataLenP
)
{
	int nb;
	struct timeval start;
	struct timeval end;

	gettimeofday(&start, NULL);

	*msgCodeP = headerP->code;
	*msgTypeP = headerP->type;

	if (dataPP == NULL)
		LM_X(1, ("dataPP == NULL"));

	if (headerP->magic != 0xFEEDC0DE)
	{
		LM_READS(ep->name.c_str(), "header", headerP, sizeof(samson::Message::Header), LmfByte);
		LM_X(1, ("bad magic number in message from %s@%s (fd: %d) ", ep->name.c_str(), ep->ip, ep->rFd));
	}

	if (headerP->dataLen != 0)
	{
		if (headerP->dataLen > 50 * 1024 * 1024)
			LM_X(1, ("Too much data! (Reading a '%s' %s from '%s', dataLens: %d, %d, %d)",
					 samson::Message::messageCode(headerP->code), samson::Message::messageType(headerP->type), ep->name.c_str(),
					 headerP->dataLen, headerP->gbufLen, headerP->kvDataLen));

		LM_T(LmtRead, ("Reading %d bytes of data", headerP->dataLen));
		if (headerP->dataLen > (unsigned int) *dataLenP)
		{
			LM_W(("Allocating extra space for the data part of incoming message '%s' from %s@%s", samson::Message::messageCode(headerP->code), ep->name.c_str(), ep->ip));
			*dataPP = (char*) malloc(headerP->dataLen);
			if (*dataPP == NULL)
				LM_X(1, ("malloc(%d)", headerP->dataLen));
		}

		LM_T(LmtRead, ("reading %d bytes of primary message data from '%s'", headerP->dataLen, ep->name.c_str()));
		nb = iomMsgPartRead(ep, "message data", (char*) *dataPP, headerP->dataLen);
		LM_T(LmtRead, ("read %d bytes DATA from '%s'", nb, ep->name.c_str()));
		if (nb == -2)
			LM_RE(-2, ("Connection closed by %s", ep->name.c_str()));
		else if (nb == -1)
			LM_RP(1, ("read %d bytes from '%s' (wanted %d bytes)", nb, ep->name.c_str(), headerP->dataLen));
		else if (nb != (int) headerP->dataLen)
			LM_E(("Read %d bytes from '%s', %d expected ...", nb, ep->name.c_str(), headerP->dataLen));

		*dataLenP = nb;

		LM_T(LmtRead, ("read %d bytes from '%s'", nb, ep->name.c_str()));
	}

	if (headerP->gbufLen != 0)
	{
		char* dataP = (char*) malloc(headerP->gbufLen + 1);

		if (dataP == NULL)
			LM_X(1, ("malloc(%d)", headerP->gbufLen));

		LM_T(LmtRead, ("reading %d bytes of google protocol buffer data", headerP->gbufLen));
		nb = iomMsgPartRead(ep, "google protocol buffer data", dataP, headerP->gbufLen);
		LM_T(LmtRead, ("read %d bytes GPROTBUF from '%s'", nb, ep->name.c_str()));
		if (nb == -2)
		{
			free(dataP);
			LM_RE(-2, ("Connection closed by %s", ep->name.c_str()));
		}
		else if (nb == -1)
		{
			free(dataP);
			LM_RP(1, ("read(%d bytes from '%s')", headerP->gbufLen, ep->name.c_str()));
		}
		else if (nb != (int) headerP->gbufLen)
		{
			free(dataP);
			LM_E(("Read %d bytes from '%s', %d expected ...", nb, ep->name.c_str(), headerP->gbufLen));
		}

		((char*) dataP)[nb] = 0;
		
		packetP->message->ParseFromArray(dataP, nb);
		if (packetP->message->IsInitialized() == false)
			LM_X(1, ("Error parsing Google Protocol Buffer of %d bytes because a message %s is not initialized!", nb, samson::Message::messageCode(headerP->code)));

		memset(dataP, 0, headerP->gbufLen + 1);
		free(dataP);
	}

	if (headerP->kvDataLen != 0)
	{
		char         name[128];
		static int   bIx = 0;

		sprintf(name, "%s:%d", ep->name.c_str(), bIx);
		++bIx;

		// By default all the input packets are for writing locally, so they are unknown.
		// Note that at the "receive" function, they can be switched to input
		packetP->buffer = engine::MemoryManager::shared()->newBuffer(name, headerP->kvDataLen, samson::MemoryOutputDisk );

		int    size   = headerP->kvDataLen;
		char*  kvBuf  = packetP->buffer->getData();
		int    size2  = packetP->buffer->getMaxSize();
		int    tot    = 0;
		int    nb;

		LM_TODO(("Should this memory buffer be freed ?"));

		LM_T(LmtRead, ("reading a KV buffer of %d bytes", size2));
		nb = iomMsgPartRead(ep, "KV Data", kvBuf, headerP->kvDataLen);
		if (nb == -2)
			LM_RE(-2, ("Connection closed by %s", ep->name.c_str()));
		else if (nb == -1)
			LM_RE(-1, ("read(%d bytes) from '%s': %s", size - tot, ep->name.c_str(), strerror(errno)));
		else if (nb != (int) headerP->kvDataLen)
			LM_E(("Read %d bytes from '%s', %d expected ...", nb, ep->name.c_str(), headerP->kvDataLen));

		packetP->buffer->setSize(nb);
	}

	int bytesRead = sizeof(samson::Message::Header) + headerP->dataLen + headerP->gbufLen + headerP->kvDataLen;

	ep->msgsIn  += 1;
	ep->bytesIn += bytesRead;

	if (bytesRead > 100)
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
			return 0;

		ep->rMbps = bytesRead / usecs;

		ep->rAccMbps = (ep->rAccMbps * ep->reads + ep->rMbps) / (ep->reads + 1);
		ep->reads += 1;
	}
	
	return 0;
}	
