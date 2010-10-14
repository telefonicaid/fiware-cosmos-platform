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
int iomMsgRead(ss::Endpoint* epP, ss::Packet* packetP, ss::Endpoint* controller, void* data, int dataLen)
{
	int        nb;
	MsgHeader  header;

    nb = read(epP->fd, &header, sizeof(header));
	
	if (nb == -1)
		LM_XP(1, ("reading header from '%s'", epP->name.c_str()));

	if (nb == 0)
	{
		LM_T(LMT_READ, ("read 0 bytes from '%s' - connection closed", epP->name.c_str()));
		epP->reset();

		if (epP == controller)
		{
			LM_W(("controller died ... trying to reconnect !"));
			while (controller->fd == -1)
			{
				controller->fd = iomConnect((const char*) controller->ip.c_str(), (unsigned short) controller->port);
				sleep(1);
			}

			controller->state = ss::Endpoint::Connected;
		}

		return -1;
	}

	LM_T(LMT_READ, ("read %d bytes from '%s'", nb, epP->name.c_str()));
	LM_READS(epP->name.c_str(), "message header", &header, sizeof(header), LmfByte);

	if (header.headerLen == 0)
		LM_W(("Got a message from '%s' with ZERO header len ...", epP->name.c_str()));
	else
	{
		char* buffer;
		int   nb;

		buffer = (char*) malloc(header.headerLen);
		if (buffer == NULL)
			LM_XP(1, ("malloc(%d)", header.headerLen));

		LM_T(LMT_READ, ("reading %d bytes Google Protocol Buffer", header.headerLen));
		nb = read(epP->fd, buffer, header.headerLen);
		if (nb == -1)
			LM_RP(1, ("read(%d bytes from '%s'", header.headerLen,  epP->name.c_str()));

		LM_T(LMT_READ, ("read %d bytes from '%s'", nb, epP->name.c_str()));
		LM_READS(epP->name.c_str(), "protocol buffer", buffer, nb, LmfByte);

		if (packetP->message.ParseFromString(std::string(buffer)) == false)
			LM_E(("ParseFromString failed!"));
	}

	if (header.dataLen == 0)
		return 0;

#if 0
	LM_T(LMT_READ, ("reading %d bytes data", header.dataLen));

	LM_T(LMT_READ, ("read %d bytes from '%s'", nb, epP->name.c_str()));
	LM_READS(epP->name.c_str(), "KV data", buffer, nb, LmfByte);
#endif
	
	return 0;
}	
