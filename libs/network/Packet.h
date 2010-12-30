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
#include "easyzlib.h"	     // zlib utility library


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

		
		void compress()
		{
		  /*
			if( buffer )
			{
				network::CompressInfo *ci = message.mutable_compress_info();
				ci->set_original_size( buffer->getSize() );
				
				
				// Destination buffer
				long int cm_max_len = EZ_COMPRESSMAXDESTLENGTH( buffer->getSize() );
				long int cm_len = cm_max_len;


				// Get a new buffer ( a little bit larger )
				Buffer *b = MemoryManager::shared()->newBuffer( "Compressed buffer", cm_len );
				
				int ans_compress = ezcompress( ( unsigned char*) b->getData(), &cm_len, ( unsigned char*) buffer->getData() , buffer->getSize() );
				assert( !ans_compress );

				MemoryManager::shared()->destroyBuffer( buffer );

				ci->set_compress_size( cm_len );
				
				b->setSize(cm_len);			// Set the size of the compressed buffer
				buffer = b;					// Assign the new buffer
				
				//std::cout << "Compressing from " << ci->original_size() << " to " << ci->compress_size() << "\n";
			}
		  */
			
		}
		
		void decompress()
		{
		  /*
			if( buffer && message.has_compress_info() )
			{
				// Decompress

				// Get the buffer with the rigth size
				long int m2_len = message.compress_info().original_size();	
				Buffer *b =  MemoryManager::shared()->newBuffer( "Decompressed buffer" , m2_len );
				b->setSize(m2_len);

				// Decompress information
				int ans_decompress = ezuncompress( (unsigned char*) b->getData(), &m2_len, (unsigned char*) buffer->getData(), buffer->getSize() );	
				assert(!ans_decompress);
				

				MemoryManager::shared()->destroyBuffer( buffer );
				buffer = b;
				
			}
		  */
		}
		
		
		~Packet()
		{
		}
	};
}

#endif
