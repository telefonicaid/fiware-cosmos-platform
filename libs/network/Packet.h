
/* ****************************************************************************
 *
 * FILE                     packet.h
 *
 * DESCRIPTION				Definition of the pakcet to be exchange in the samson-ecosystem
 *
 * ***************************************************************************/

#pragma once

#include "samson.pb.h"
#include <assert.h>		// assert

namespace ss {
	
	class Endpoint;
	
	class PacketBuffer
	{
		char *data;
		size_t length;

	public:
		
		PacketBuffer()
		{
			data = NULL;
			length = 0;
		}
		
		~PacketBuffer()
		{
			free();
		}

		void initPacketBuffer( size_t _length )
		{
			assert( data == NULL);
			data = (char*)malloc(_length);
			length = _length;
		}
		
		void free()
		{
			if( data )
			{
				::free( data );
				data = NULL;
				length = 0;
			}
		}		
		
		char *getDataPointer()
		{
			return data;
		}
		
		size_t getLength()
		{
			return length;
		}
		
	};
	
	/** 
	 Unique packet type sent over the network between controller, samson and delilah
	 */

	class Packet
	{
	public:
		typedef enum MessageCode
		{
			Hello,
			WorkerVector
		} MessageCode;

		char* msgCodeName(MessageCode);

		network::Message message;		// Message with necessary fields ( codified with Google Protocol Buffers )
		PacketBuffer buffer;
	
		Packet() {}

		Packet(MessageCode c)
		{
			setMessageCode(c);
		}
	
		
		/**
		 Add information to the header
		 */
		void setMessageCode(MessageCode c);
		void addEndpoint( Endpoint *e );
		void addEndPoints( std::vector<Endpoint> &_es );
		
		/**
		 Get information from the header
		 */
		
		int getNumEndpoints();
		Endpoint getEndpoint( int i );
		
		
		/** 
		 Debug string with information about the packet
		 */
		std::string str();
	};
	
}
