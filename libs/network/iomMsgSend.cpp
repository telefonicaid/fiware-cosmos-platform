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
int iomMsgSend(int fd, char* name, ss::Packet* packetP, char* sender, void* data, int dataLen)
{
	MsgHeader     header;
	struct iovec  ioVec[3];
	int           vecs = 3;
	ssize_t       s;
	char*         outputVec;

	ioVec[0].iov_len = 0;
	ioVec[1].iov_len = 0;
	ioVec[2].iov_len = 0;

	if ((data != NULL) || (dataLen != 0))
		LM_X(1, ("try to send %d bytes of KV data to '%s'", dataLen, name));

	packetP->message.set_sendername(sender);

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
	   LM_RE(1, ("SerializeToArray failed"));

	LM_M(("outputVec[0-4]: 0x%x, 0x%x, 0x%x, 0x%x",
		  outputVec[0] & 0xFF,
		  outputVec[1] & 0xFF,
		  outputVec[2] & 0xFF,
		  outputVec[3] & 0xFF
			));

	ioVec[1].iov_base  = outputVec;
	ioVec[1].iov_len   = packetP->message.ByteSize();
	
	s = writev(fd, ioVec, vecs);
	if (s == -1)
	{
		LM_P(("writev(%s)", name));
		return -1;
	}

	if (s == 0)
	{
		LM_E(("writev(%s) returned ZERO bytes written", name));
		return -1;
	}

	LM_T(LMT_WRITE, ("written %d bytes to '%s'", s, name));
	LM_WRITES(name, "message header",  ioVec[0].iov_base, ioVec[0].iov_len, LmfByte);
	LM_WRITES(name, "protocol buffer", ioVec[1].iov_base, ioVec[1].iov_len, LmfByte);

	if ((vecs == 3) && (ioVec[2].iov_len != 0))
		LM_WRITES(name, "KV data",         ioVec[2].iov_base, ioVec[2].iov_len, LmfByte);

	return 0;
}	
