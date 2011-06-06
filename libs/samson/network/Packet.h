#ifndef PACKET_H
#define PACKET_H

/* ****************************************************************************
*
* FILE                      packet.h
*
* DESCRIPTION				Definition of the packet to be exchanged in the samson-ecosystem
*
*/
#include "logMsg/logMsg.h"               // LM_TODO()

#include "samson/common/samson.pb.h"     // google protocol buffers
#include "engine/MemoryManager.h"        // MemoryManager
#include "engine/Engine.h"               // engine::Engine
#include "engine/Buffer.h"               // engine::Buffer
#include "Endpoint.h"                    // Endpoint
#include "easyzlib.h"                    // zlib utility library
#include "Message.h"                     // samson::MessageType 
#include "samson/common/EnvironmentOperations.h"        // str( network::Message* )


namespace samson
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
		
		Packet(Message::MessageType type, Message::MessageCode code, void* _dataP = NULL, int _dataLen = 0)
		{
			msgType  = type;
			msgCode  = code;
			dataP    = _dataP;
			dataLen  = _dataLen;
			buffer   = NULL;
			message  = new network::Message();
			fromId   = -9;
		};

		Packet(Message::MessageCode _msgCode)
		{
			msgCode = _msgCode;
			msgType = Message::Evt;
			buffer  = NULL;
			dataLen = 0;
			dataP   = NULL;
			message = new network::Message();
			fromId  = -9;
		};

		Packet(Packet* p)
		{
			// Copy the message type
			msgCode = p->msgCode;
			msgType = p->msgType;
			fromId  = p->fromId;
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
        
        
        // Debug string
        std::string str()
        {
            std::ostringstream output;
            output << "Packet " << messageCode( msgCode ) << " " << strMessage( message );
            if ( buffer )
                output << "[ Buffer " << au::Format::string(  buffer->getSize() ) << "/" << au::Format::string(  buffer->getMaxSize() ) << " ]" ;
            else
                output << "[ No buffer ]";
            return output.str();
        }
        
	};
}

#endif
