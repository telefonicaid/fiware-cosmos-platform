/* ****************************************************************************
*
* FILE                  Packet.cpp - packet to send/receive across SAMSON nodes / uploader
*
*
*/
#include "samsonTuning.h"        /* NUM_SETS_PER_STORAGE                     */
#include "samson.pb.h"           /* protocol                                 */
#include "KVSetBuffer.h"         /* KVSetTxTBuffer                           */
#include "KVSet.h"               /* KVSet                                    */
#include "KVManager.h"           /* KVManager                                */
#include "Packet.h"              /* Own interface                            */



namespace ss {

	struct MessageHeader
	{
		size_t message_length;	// Lenght of the message in protocol-buffer
		size_t buffer_length;	// Length of the data-buffer
	};



	bool Packet::read(au::Socket *socket)
	{
		MessageHeader  header;
		char *         message_buffer;
		
		// Read header
		ssize_t size = socket->recv_waiting((char*) &header, sizeof(MessageHeader));
		
		message_buffer = (char*) malloc( header.message_length );
		
		if( size != sizeof( MessageHeader) )
			return false;
		
		// Message ( protocol buffers )
		socket->recv_waiting(message_buffer , header.message_length );
		
		// Parse from the buffer
		message.ParseFromArray(message_buffer, header.message_length);
		
		if( header.buffer_length > 0 )
		{
			// Create a txt based document
			KVSetTxTBuffer *kvSetBuffer = new KVSetTxTBuffer(header.buffer_length);
			
			// Get the buffer where income socket data is stored
			char *buffer = kvSetBuffer->getTxtDataBuffer();

			// Read data into the dedicated buffer
			socket->recv_waiting( buffer  , header.buffer_length );
			
			kvSetBuffer->close();
			
			KVSet *set = KVManager::shared()->addKVSet( 0, kvSetBuffer );
			KVManager::shared()->addKVSetToKVQueue( 0, set , message.queue() , rand()%NUM_SETS_PER_STORAGE );
			
			delete kvSetBuffer;
		}
		
		free( message_buffer );
		
		return true;
	}
	
	void Packet::send( au::Socket *socket  )
	{
		MessageHeader header;
		header.message_length = message.ByteSize();
		header.buffer_length = dataBuffer.size;

		// serialize the message
		char *message_buffer = (char*) malloc( header.message_length );		
		
		assert(message.IsInitialized());
		bool ans = message.SerializeToArray(message_buffer, header.message_length );
		if (!ans)
		{
			LOG_ERROR(("error serializing message"));
			assert(0);
		}

		// Send content to the socket
		socket->send( (char*)&header, sizeof( header ) );
		socket->send(message_buffer , header.message_length );
		if( dataBuffer.size > 0 )
			socket->send( dataBuffer.buffer  , dataBuffer.size );

		
		free( message_buffer );
	}	
}
