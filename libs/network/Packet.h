#ifndef PACKET_H
#define PACKET_H

/* ****************************************************************************
*
* FILE                      packet.h
*
* DESCRIPTION				Definition of the packet to be exchanged in the samson-ecosystem
*
*/

#include "samson.pb.h"       // google protocol buffers
#include "engine/MemoryManager.h"   // MemoryManager
#include "Endpoint.h"        // Endpoint
#include "easyzlib.h"	     // zlib utility library
#include "logMsg.h"			 // LM_TODO()
#include "Message.h"		 // ss::MessageType 
#include "engine/Engine.h"			 // engine::Engine
#include "engine/Buffer.h"          // engine::Buffer

namespace ss {
	
	class Endpoint;

	// This has been removed for simplicity. Files are now uploaded with a particular extension to indicate the compression mode
	
	/*
	typedef struct 
	{
		int compressed;			// Information about the compression format ( 0 not compressed , 1 gzip compression )
		size_t original_size;	// Size of the original buffer
		size_t compressed_size;	// Size of the compressed version of the buffer
		int magic_number;		// Magic number to check everything is correct
		
		
		void init( int _compressed )
		{
			compressed = _compressed;
			original_size = 0;
			compressed_size = 0;
			magic_number = 8345762;
		}
		
		bool check( )
		{
			return (magic_number == 8345762);
		}
		
	} BufferHeader;
	*/
	 
	 
	/** 
	 Unique packet type sent over the network between controller, samson and delilah
	 */

	class Packet : public engine::Object
	{
	public:
		
		int fromId;						// Identifier of the sender of this packet
		Message::MessageCode msgCode;	// Message code ( sent in the header of the network interface )		
		network::Message *message;		// Message with necessary fields ( codified with Google Protocol Buffers )
		engine::Buffer* buffer;			// Data for key-values
		
		Packet() 
		{
			buffer = NULL;
			message = new network::Message();
		};

        
		Packet( Packet * p ) 
		{
            // Copy the buffer ( if any )
            if( p->buffer )
            {
                buffer = engine::MemoryManager::shared()->newBuffer("", p->buffer->getSize(), p->buffer->getType() );
                memcpy(buffer->getData(), p->buffer->getData(), p->buffer->getSize() );
                buffer->setSize( p->buffer->getSize() );
            }
            else
                buffer = NULL;

            // Google protocol buffer message
			message = new network::Message();
            message->CopyFrom( *p->message );
            
		};
        
        
		~Packet()
		{
			delete message;
		}
		
		
		
	};
}

#endif
