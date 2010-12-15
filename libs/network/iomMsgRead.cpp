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

#include "logMsg.h"             // LM_*
#include "networkTraceLevels.h" // LMT_NWRUN, ...

#include "Message.h"            // MessageType, Code, etc.
#include "Packet.h"             // Packet
#include "Buffer.h"             // Buffer
#include "MemoryManager.h"      // MemoryManager
#include "iomMsgRead.h"         // Own interface



/* ****************************************************************************
*
* iomMsgRead - read a message from an endpoint
*/
int iomMsgRead
(
	int                        fd,
	const char*                from,
	ss::Message::MessageCode*  msgCodeP,
	ss::Message::MessageType*  msgTypeP,
	void**                     dataPP,
	int*                       dataLenP,
	ss::Packet*                packetP,
	void*                      kvData,
	int*                       kvDataLenP
)
{
	int                  nb;
	ss::Message::Header  header;
	
    nb = read(fd, &header, sizeof(header));
	
	if (nb == -1)
		LM_RE(1, ("reading header from '%s'", from));

	if (nb == 0)
	{
		LM_T(LMT_MSG, ("read 0 bytes from '%s' - connection closed", from));
		return -2;
	}

	if (nb != sizeof(header))
		LM_RE(1, ("reading header from '%s' - read only %d bytes (need %d)", from, nb, sizeof(header)));

	if (header.magic != 0xFEEDC0DE)	
		LM_X(1, ("Bad magic number in header (0x%x)", header.magic));

	*msgCodeP = header.code;
	*msgTypeP = header.type;

	LM_T(LMT_MSG, ("read %d bytes of '%s' %s header from '%s' (fd %d)",
				   nb, messageCode(header.code), messageType(header.type), from, fd));

	LM_READS(from, "message header", &header, sizeof(header), LmfByte);

	if (header.dataLen != 0)
	{
		int nb;

		if (header.dataLen > (unsigned int) *dataLenP)
		{
			*dataPP = (char*) malloc(header.dataLen);
			if (*dataPP == NULL)
				LM_X(1, ("malloc(%d)", header.dataLen));
		}

		LM_T(LMT_MSG, ("reading %d bytes of primary message data", header.dataLen));
		nb = read(fd, *dataPP, header.dataLen);
		LM_T(LMT_MSG, ("read %d bytes DATA from '%s'", nb, from));
		if (nb == -1)
			LM_RP(1, ("read %d bytes from '%s'", header.dataLen, from));
		LM_T(LMT_MSG, ("read %d bytes of primary message data", nb));

		if (nb != (int) header.dataLen)
			LM_E(("Read %d bytes, %d expected ...", nb, header.dataLen));

		*dataLenP = nb;

		LM_T(LMT_MSG, ("read %d bytes from '%s'", nb, from));
		LM_READS(from, "primary data", *dataPP, nb, LmfByte);
	}

	if (header.gbufLen != 0)
	{
		void* dataP = (void*)  malloc(header.gbufLen + 1);

		if (dataP == NULL)
			LM_X(1, ("malloc(%d)", header.gbufLen));

		LM_T(LMT_MSG, ("reading %d bytes of google protocol buffer data", header.gbufLen));
        nb = read(fd, dataP, header.gbufLen);
        if (nb == -1)
			LM_RP(1, ("read(%d bytes from '%s')", header.gbufLen, from));

		if (nb != (int) header.gbufLen)
			LM_X(1, ("read %d bytes instead of %d", nb, header.gbufLen));

		((char*) dataP)[nb] = 0;

		if (packetP->message.ParseFromArray(dataP, nb) == false)
			LM_X(1, ("Error parsing Google Protocol Buffer!"));

		LM_READS(from, "google protocol buffer", dataP, nb, LmfByte);
	}

	if (header.kvDataLen != 0)
	{
		char         name[128];
		static int   bIx = 0;

		sprintf(name, "Buffer%d", bIx);
		++bIx;

		packetP->buffer = ss::MemoryManager::shared()->newBuffer(name, header.kvDataLen);

		int    size  = header.kvDataLen;
		char*  kvBuf = packetP->buffer->getData();
		int    tot   = 0;
		int    nb;

		while (tot < size)
		{
			// msgAwait()
			nb = read(fd, &kvBuf[tot], size - tot);
			LM_T(LMT_MSG, ("read %d bytes KVDATA from '%s'", tot, from));
			if (nb == -1)
				LM_RE(-1, ("read(%d bytes) from '%s': %s", size - tot, from, strerror(errno)));
			tot += nb;
		}

		packetP->buffer->setSize(tot);
	}

	return 0;
}	



/* ****************************************************************************
*
* iomMsgRead - read a message from an endpoint
*/
int iomMsgRead
(
	ss::Endpoint*              ep,
	ss::Message::Header*       headerP,
	ss::Message::MessageCode*  msgCodeP,
	ss::Message::MessageType*  msgTypeP,
	void**                     dataPP,
	int*                       dataLenP,
	ss::Packet*                packetP,
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

	if (headerP->dataLen != 0)
	{
		if (headerP->dataLen > 1000)
			LM_X(1, ("Reading a '%s' %s from '%s' (dataLens: %d, %d, %d)",
					 ss::Message::messageCode(headerP->code), ss::Message::messageType(headerP->type), ep->name.c_str(),
					 headerP->dataLen, headerP->gbufLen, headerP->kvDataLen));

		LM_T(LMT_MSG, ("Reading %d bytes of data", headerP->dataLen));
		if (headerP->dataLen > (unsigned int) *dataLenP)
		{
			LM_W(("Allocating extra space for message"));
			*dataPP = (char*) malloc(headerP->dataLen);
			if (*dataPP == NULL)
				LM_X(1, ("malloc(%d)", headerP->dataLen));
		}

		LM_T(LMT_MSG, ("reading %d bytes of primary message data", headerP->dataLen));
		nb = read(ep->rFd, *dataPP, headerP->dataLen);
		LM_T(LMT_MSG, ("read %d bytes DATA from '%s'", nb, ep->name.c_str()));
		if (nb == -1)
			LM_RP(1, ("read %d bytes from '%s'", headerP->dataLen, ep->name.c_str()));
		LM_T(LMT_MSG, ("read %d bytes of primary message data", nb));

		if (nb != (int) headerP->dataLen)
			LM_E(("Read %d bytes from '%s', %d expected ...", nb, ep->name.c_str(), headerP->dataLen));

		*dataLenP = nb;

		LM_T(LMT_MSG, ("read %d bytes from '%s'", nb, ep->name.c_str()));
		LM_READS(ep->name.c_str(), "primary data", *dataPP, nb, LmfByte);
	}

	if (headerP->gbufLen != 0)
	{
		char* dataP = (char*) malloc(headerP->gbufLen + 1);
		int   tot   = 0;

		if (dataP == NULL)
			LM_X(1, ("malloc(%d)", headerP->gbufLen));

		LM_T(LMT_MSG, ("reading %d bytes of google protocol buffer data", headerP->gbufLen));
		while (tot < (int) headerP->gbufLen)
		{
			nb = read(ep->rFd, &dataP[tot], headerP->gbufLen - tot);
			LM_T(LMT_MSG, ("read %d bytes GPROTBUF from '%s'", nb, ep->name.c_str()));
			if (nb == -1)
				LM_RP(1, ("read(%d bytes from '%s')", headerP->gbufLen, ep->name.c_str()));
			tot += nb;
		}

		((char*) dataP)[tot] = 0;
		if (packetP->message.ParseFromArray(dataP, tot) == false)
			LM_X(1, ("Error parsing Google Protocol Buffer!"));

		LM_READS(ep->name.c_str(), "google protocol buffer", dataP, tot, LmfByte);
	}

	if (headerP->kvDataLen != 0)
	{
		char         name[128];
		static int   bIx = 0;

		sprintf(name, "%s:%d", ep->name.c_str(), bIx);
		++bIx;

		packetP->buffer = ss::MemoryManager::shared()->newBuffer(name, headerP->kvDataLen);

		int    size   = headerP->kvDataLen;
		char*  kvBuf  = packetP->buffer->getData();
		int    size2  = packetP->buffer->getMaxSize();
		int    tot    = 0;
		int    nb;

		LM_T(LMT_MSG, ("reading a KV buffer of %d bytes", size2));
		while (tot < size)
		{
			// msgAwait() ... ?
			LM_T(LMT_MSG, ("trying to read %d bytes of KV buffer", size - tot));
			nb = read(ep->rFd, &kvBuf[tot], size - tot);
			LM_T(LMT_MSG, ("read %d bytes of KV buffer", nb));

			if (nb == -1)
				LM_RE(-1, ("read(%d bytes) from '%s': %s", size - tot, ep->name.c_str(), strerror(errno)));
			else if (nb == 0)
				LM_RE(-2, ("endpoint '%s' seems to have closed the connection", ep->name.c_str()));
			tot += nb;
		}

		packetP->buffer->setSize(tot);
	}

	int bytesRead = sizeof(ss::Message::Header) + headerP->dataLen + headerP->gbufLen + headerP->kvDataLen;

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
