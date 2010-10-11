
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
	 Unique packet type sent over the network between controller, samson and delailah
	 */

	class Packet
	{

	public:
		network::Message message;		// Message with necessary fields ( codified with Google Protocol Buffers )
		PacketBuffer buffer;
	
		Packet(){}
	
		/** 
		 Debug string with information about the packet
		 */
		std::string str();
		
	};
	
}
