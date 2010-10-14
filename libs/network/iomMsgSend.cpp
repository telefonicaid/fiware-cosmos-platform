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

#include "logMsg.h"             // LM_*
#include "networkTraceLevels.h" // LMT_NWRUN, ...

#include "Endpoint.h"           // Endpoint
#include "Packet.h"             // Packet
#include "MsgHeader.h"          // MsgHeader
#include "iomMsgSend.h"         // Own interface



/* ****************************************************************************
*
* iomMsgSend - send a message to an endpoint
*/
int iomMsgSend(ss::Endpoint* epP, ss::Packet* packetP, void* data, int dataLen)
{
	MsgHeader     header;
	struct iovec  ioVec[3];
	int           vecs = 3;
	ssize_t       s;
	char*         outputVec;

	header.headerLen = packetP->message.ByteSize();
	header.dataLen   = dataLen;
	outputVec        = (char*) malloc(header.headerLen);

	if (outputVec == NULL)
		LM_XP(1, ("malloc(%d)", header.headerLen));

	if (data == NULL)
	{
		if (dataLen != 0)
			LM_X(1, ("data == NULL while dataLen == %d - doesn't match. Fix it!", dataLen));
		vecs = 2;
	}
	else
	{
		if (dataLen == 0)
			LM_X(1, ("data != NULL while dataLen == %d - doesn't match. Fix it!", dataLen));
		ioVec[2].iov_base = data;
		ioVec[2].iov_len  = dataLen;
	}

	ioVec[0].iov_base  = &header;
	ioVec[0].iov_len   = sizeof(header);

	if (packetP->message.SerializeToArray(outputVec, header.headerLen) == false)
	   LM_X(1, ("SerializeToArray failed"));

	ioVec[1].iov_base  = outputVec;
	ioVec[1].iov_len   = packetP->message.ByteSize();
	
	s = writev(epP->fd, ioVec, vecs);
	if (s == -1)
	{
		LM_P(("writev(%s)", epP->name.c_str()));
		close(epP->fd);
		epP->fd    = -1;
		epP->state = ss::Endpoint::Free;
		return -1;
	}

	if (s == 0)
	{
		LM_E(("writev(%s) returned ZERO bytes written", epP->name.c_str()));
		close(epP->fd);
		epP->fd    = -1;
		epP->state = ss::Endpoint::Free;
		return -1;
	}

	LM_T(LMT_WRITE, ("written %d bytes to '%s'", s, epP->name.c_str()));
	return 0;
}	
