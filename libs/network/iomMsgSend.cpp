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

#include "Message.h"            // Message::Header, MessageCode, MessageType, ...
#include "Packet.h"             // Packet
#include "iomMsgSend.h"         // Own interface

#include "Buffer.h"				// ss::Buffer


/* ****************************************************************************
*
* iomMsgSend - send a message to an endpoint
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

	memset(&header, 0, sizeof(header));
	memset(ioVec, 0, sizeof(ioVec));

	header.code        = code;
	header.type        = type;

	ioVec[0].iov_base  = &header;
	ioVec[0].iov_len   = sizeof(header);
	
	if (dataLen != 0)
	{
		if (data == NULL)
			LM_X(1, ("dataLen %d, but data is a NULL pointer ...", dataLen));
		
		header.dataLen        = dataLen;
		ioVec[vecs].iov_base  = data;
		ioVec[vecs].iov_len   = dataLen;
		
		LM_WRITES(to, "data", ioVec[vecs].iov_base, ioVec[vecs].iov_len, LmfByte);
		++vecs;
	}

	if (packetP != NULL)
	{
		char* outputVec;

		header.gbufLen = packetP->message.ByteSize();

		outputVec = (char*) malloc(header.gbufLen);
		if (outputVec == NULL)
			LM_XP(1, ("malloc(%d)", header.gbufLen));

		if (packetP->message.SerializeToArray(outputVec, header.gbufLen) == false)
			LM_RE(1, ("SerializeToArray failed"));

		ioVec[vecs].iov_base  = outputVec;
		ioVec[vecs].iov_len   = packetP->message.ByteSize();

		LM_WRITES(to, "google pbuffer", ioVec[vecs].iov_base, ioVec[vecs].iov_len, LmfByte);
		++vecs;
	}
	else
		LM_M(("NULL Google protocol buffer"));

	if (packetP && packetP->buffer != 0)
	{

		header.kvDataLen      = packetP->buffer->getSize();
		ioVec[vecs].iov_base  = packetP->buffer->getData();
		ioVec[vecs].iov_len   = packetP->buffer->getSize();
		
		LM_WRITES(to, "KV data", ioVec[vecs].iov_base, ioVec[vecs].iov_len, LmfByte);
		++vecs;
	}

	LM_T(LMT_MSG, ("Sending '%s' %s to '%s', data bytes: { data: %d, gbuf: %d, kv: %d }",
				   messageCode(code), messageType(type), to, header.dataLen, header.gbufLen, header.kvDataLen));

	s = writev(fd, ioVec, vecs);
	if (s == -1)
	{
		LM_P(("writev(%s)", to));
		return -1;
	}

	if (s == 0)
	{
		LM_E(("writev(%s) returned ZERO bytes written", to));
		return -1;
	}

	if (s != (ssize_t) (ioVec[0].iov_len + ioVec[1].iov_len + ioVec[2].iov_len + ioVec[3].iov_len))
		LM_X(1, ("written only %d bytes, wanted to write %d (%d + %d + %d + %d)",
				 s, 
				 ioVec[0].iov_len + ioVec[1].iov_len + ioVec[2].iov_len + ioVec[3].iov_len,
				 ioVec[0].iov_len, ioVec[1].iov_len, ioVec[2].iov_len, ioVec[3].iov_len));

	LM_T(LMT_WRITE, ("written %d bytes to '%s' (fd %d)", s, to, fd));

	LM_WRITES(to, "message header",  ioVec[0].iov_base, ioVec[0].iov_len, LmfByte);
	if (dataLen != 0)
		LM_WRITES(to, "message data",  ioVec[1].iov_base, ioVec[1].iov_len, LmfByte);

	return 0;
}	
