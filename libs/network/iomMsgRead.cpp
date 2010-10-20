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

#include "Endpoint.h"           // Endpoint
#include "Packet.h"             // Packet
#include "MsgHeader.h"          // MsgHeader
#include "iomConnect.h"         // iomConnect
#include "iomMsgRead.h"         // Own interface



/* ****************************************************************************
*
* iomMsgRead - read a message from an endpoint
*/
int iomMsgRead(int fd, char* name, ss::Packet* packetP, void* data, int dataLen)
{
	int        nb;
	MsgHeader  header;

    nb = read(fd, &header, sizeof(header));
	
	if (nb == -1)
		LM_RE(1, ("reading header from '%s'", name));

	if (nb == 0)
	{
		LM_T(LMT_READ, ("read 0 bytes from '%s' - connection closed", name));
		return -2;
	}

	LM_T(LMT_READ, ("read %d bytes from '%s' (fd %d)", nb, name, fd));
	LM_READS(name, "message header", &header, sizeof(header), LmfByte);

	if (header.headerLen == 0)
		LM_W(("Got a message from '%s' with ZERO header len ...", name));
	else
	{
		int   nb;
		char* buffer;

		buffer = (char*) malloc(header.headerLen);
		if (buffer == NULL)
			LM_X(1, ("malloc(%d)", header.headerLen));

		LM_T(LMT_READ, ("reading %d bytes Google Protocol Buffer", header.headerLen));
		nb = read(fd, buffer, header.headerLen);
		if (nb == -1)
			LM_RP(1, ("read(%d bytes from '%s'", header.headerLen,  name));

	LM_M(("buffer[0-4]: 0x%x, 0x%x, 0x%x, 0x%x",
		  buffer[0] & 0xFF,
		  buffer[1] & 0xFF,
		  buffer[2] & 0xFF,
		  buffer[3] & 0xFF
			));

		LM_T(LMT_READ, ("read %d bytes from '%s'", nb, name));
		LM_READS(name, "protocol buffer", buffer, nb, LmfByte);

		if (packetP->message.ParseFromArray(buffer, nb) == false)
			LM_RE(1, ("ParseFromString failed!"));
	}

	if (header.dataLen == 0)
		return 0;

#if 0
	LM_T(LMT_READ, ("reading %d bytes data", header.dataLen));

	LM_T(LMT_READ, ("read %d bytes from '%s'", nb, name));
	LM_READS(name, "KV data", buffer, nb, LmfByte);
#endif
	
	return 0;
}	
