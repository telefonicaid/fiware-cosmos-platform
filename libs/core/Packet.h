#ifndef SAMSON_PACKET_H
#define SAMSON_PACKET_H

/* ****************************************************************************
*
* FILE                  Packet.h - packet to send/receive across SAMSON nodes / uploader
*
*
*/
#include "samson.pb.h"           /* protocol                                 */
#include "Sockets.h"             /* au::Socket                               */
#include "DataBuffer.h"          /* DataBuffer                               */



namespace ss {

	class Packet
	{
	public:
		protocol::Message message;      // Message indicating queue / format / etc..
		DataBuffer        dataBuffer;   // Buffer of data read/sent to the network
		
		Packet()
		{
			dataBuffer.buffer = NULL;
			dataBuffer.size   = 0;
		}
		
		~Packet()
		{
		}
	
		bool read( au::Socket *socket );
		void send( au::Socket *socket );		
	};
}

#endif
