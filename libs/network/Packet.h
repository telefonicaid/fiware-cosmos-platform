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

#include "samson.pb.h"       // google protocol buffers
#include "Endpoint.h"        // Endpoint



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
			Hello        = 0x51,
			WorkerVector,
			DalilahCommand,					// Message sent from dalilah to controller
			DalilahCommandResponse,			// Answer from the controller
			WorkerTask,						// Task sent from controller to each worker
			WorkerTaskConfirmation			// Confirmation sent form workers to the controller
		} MessageCode;

		typedef enum MessageType
		{
		   Req = 0x61,
		   Ack,
		   Nak
		} MessageType;

		char* msgCodeName(MessageCode);

		network::Message message;		// Message with necessary fields ( codified with Google Protocol Buffers )
		PacketBuffer buffer;
	
		Packet() {}

		Packet(MessageCode c)
		{
			messageCodeSet(c);
		}


		void      messageCodeSet(MessageCode code);
		void      messageTypeSet(MessageType type);

		int messageCodeGet();
		
		void      endpointAdd(Endpoint* e);
		Endpoint  endpointGet(int i);
		void      endpointVectorAdd(std::vector<Endpoint>& es);
		int       endpointVecSize(void);

		void      helloAdd(char*  name, int  connectedWorkers, Endpoint::Type  type, char*   ip = NULL, unsigned short  port = 0);
		void      helloGet(char** name, int* connectedWorkers, Endpoint::Type* typeP, char** ip,        unsigned short* port);

		/**
		 Debug string with information about the packet
		 */
		std::string str();
	};
}

#endif
