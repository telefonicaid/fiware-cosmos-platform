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

#include "logMsg.h"             // LM_*
#include "networkTraceLevels.h" // LMT_NWRUN, ...

#include "Message.h"            // MessageType, Code, etc.
#include "Packet.h"             // Packet
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
		LM_T(LMT_READ, ("read 0 bytes from '%s' - connection closed", from));
		return -2;
	}

	*msgCodeP = header.code;
	*msgTypeP = header.type;

	LM_T(LMT_MSG, ("read %d bytes of '%s' %s header from '%s' (fd %d)",
				   nb, messageCode(header.code), messageType(header.type), from, fd));

	LM_READS(from, "message header", &header, sizeof(header), LmfByte);

	if (header.dataLen != 0)
	{
		int   nb;

		if (header.dataLen > (unsigned int) *dataLenP)
		{
			*dataPP = (char*) malloc(header.dataLen);
			if (*dataPP == NULL)
				LM_X(1, ("malloc(%d)", header.dataLen));
		}

		LM_T(LMT_READ, ("reading %d bytes of primary message data", header.dataLen));
		nb = read(fd, *dataPP, header.dataLen);
		if (nb == -1)
			LM_RP(1, ("read(%d bytes from '%s'", header.dataLen,  from));

		LM_T(LMT_READ, ("read %d bytes from '%s'", nb, from));
		LM_READS(from, "primary data", *dataPP, nb, LmfByte);
	}

	if (header.gbufLen != 0)
	{
		void* dataP = (void*)  malloc(header.gbufLen);

		if (dataP == NULL)
			LM_X(1, ("malloc(%d)", header.gbufLen));

        LM_T(LMT_READ, ("reading %d bytes of google protocol buffer data", header.gbufLen));
        nb = read(fd, dataP, header.gbufLen);
        if (nb == -1)
			LM_RP(1, ("read(%d bytes from '%s')", header.gbufLen, from));

		if (packetP->message.ParseFromArray(dataP, nb) == false)
			LM_X(1, ("ParseFromString failed!"));

		LM_READS(from, "google protocol buffer", dataP, nb, LmfByte);
	}

	if (header.kvDataLen != 0)
		LM_X(1, ("Sorry, will not read KV data form '%s' - not implemented", from));

	return 0;
}	
