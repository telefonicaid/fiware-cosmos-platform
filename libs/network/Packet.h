#ifndef PACKET_H
#define PACKET_H

/* ****************************************************************************
*
* FILE                      packet.h
*
* DESCRIPTION				Definition of the packet to be exchanged in the samson-ecosystem
*
*/
#include <assert.h>	         // assert

#include "MemoryManager.h"   // MemoryManager
#include "samson.pb.h"       // google protocol buffers
#include "Endpoint.h"        // Endpoint



namespace ss {
	
	class Endpoint;
	class Buffer;
	

	
	/** 
	 Unique packet type sent over the network between controller, samson and delilah
	 */

	class Packet
	{
	public:
		network::Message message;		// Message with necessary fields ( codified with Google Protocol Buffers )
		Buffer* buffer;					// Data for key-values
		
		Packet() 
		{
			buffer = NULL;
		};

		~Packet()
		{
		}
	};
}

#endif
