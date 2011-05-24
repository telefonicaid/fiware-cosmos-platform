#ifndef PACKET_H
#define PACKET_H

/* ****************************************************************************
*
* FILE                      packet.h
*
* DESCRIPTION				Definition of the packet to be exchanged in the samson-ecosystem
*
*/
#include "samson.pb.h"            // google protocol buffers
#include "engine/MemoryManager.h" // MemoryManager
#include "Endpoint.h"             // Endpoint
#include "easyzlib.h"             // zlib utility library
#include "logMsg.h"               // LM_TODO()
#include "Message.h"              // ss::MessageType 
#include "engine/Engine.h"        // engine::Engine
#include "engine/Buffer.h"        // engine::Buffer

namespace ss
{
	class Endpoint;

	/** 
	 Unique packet type sent over the network between controller, samson and delilah
	 */

	class Packet : public engine::Object
	{
	public:
		int                   fromId;   // Identifier of the sender of this packet
		Message::MessageType  msgType;  // Message type (sent in the header of the network interface)
		Message::MessageCode  msgCode;  // Message code (sent in the header of the network interface)
		void*                 dataP;    // Raw data, mostly used for signaling
		int                   dataLen;  // Length of raw data
		network::Message*     message;  // Message with necessary fields (codified using Google Protocol Buffers)
		engine::Buffer*       buffer;   // Data for key-values
		
		Packet(Message::MessageType type, Message::MessageCode code, void* _dataP, int _dataLen)
		{
			msgType  = type;
			msgCode  = code;
			dataP    = _dataP;
			dataLen  = _dataLen;
			buffer   = NULL;
			message  = NULL;
		};

		Packet(Message::MessageCode _msgCode)
		{
			msgCode = _msgCode;
			buffer  = NULL;
			dataLen = 0;
			dataP   = NULL;
			message = new network::Message();
		};

		Packet(Packet* p)
		{
			// Copy the message type
			msgCode = p->msgCode;
			dataLen = 0;
			dataP   = NULL;

			// Copy the buffer (if any)
			if (p->buffer)
			{
				buffer = engine::MemoryManager::shared()->newBuffer("", p->buffer->getSize(), p->buffer->tag );
				memcpy(buffer->getData(), p->buffer->getData(), p->buffer->getSize() );
				buffer->setSize( p->buffer->getSize() );
			}
			else
				buffer = NULL;

			// Google protocol buffer message
			message = new network::Message();
			message->CopyFrom(*p->message);
		};


		~Packet()
		{
			// Note: Not remove buffer since it can be used outside the scope of this packet
			delete message;
		}
	};
}

#endif
