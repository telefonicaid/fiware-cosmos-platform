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
#include "MemoryManager.h"   // MemoryManager
#include "Endpoint.h"        // Endpoint
#include "easyzlib.h"	     // zlib utility library
#include "logMsg.h"			 // LM_TODO()
#include "Message.h"		 // ss::MessageType 
#include "Engine.h"			 // ss::Engine

namespace ss {
	
	class Endpoint;
	class Buffer;
	

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

	class Packet
	{
	public:
		
		int fromId;						// Identifier of the sender of this packet
		Message::MessageCode msgCode;	// Message code ( sent in the header of the network interface )		
		network::Message *message;		// Message with necessary fields ( codified with Google Protocol Buffers )
		Buffer*         buffer;			// Data for key-values
		
		Packet() 
		{
			buffer = NULL;
			message = new network::Message();
		};

		~Packet()
		{
			delete message;
		}
		

		static Buffer* compressBuffer( Buffer *buffer )
		{
			
			// Check input buffer format
			//BufferHeader *_header = (BufferHeader*) buffer->getData();
			//assert( _header->check() );
			//assert( _header->compressed == 0);
			
			// Create the header for the output buffer
			//BufferHeader header;						// Header to write as header output
			//header.init( 1 );							// Init header ( and magic number )

			// Get the original size
			size_t original_size = buffer->getSize();
			
			// Destination buffer
			long int cm_max_len = EZ_COMPRESSMAXDESTLENGTH( original_size );
			long int cm_len = cm_max_len;

			// Get a new buffer ( a little bit larger )
			Buffer *b = Engine::shared()->memoryManager.newBuffer( "Compressed buffer", cm_len , buffer->getType() );
			
			int ans_compress = ezcompress( ( unsigned char*) ( b->getData() ), &cm_len, ( unsigned char*) (buffer->getData() ) , original_size );
			if (ans_compress != 0)
				LM_X(1, ("ezcompress error"));

			b->setSize(cm_len);			// Set the size of the compressed buffer

			
			// Create a new buffer with the rigth size
			Buffer *b2 = Engine::shared()->memoryManager.newBuffer( "Compressed buffer2", b->getSize() , buffer->getType()	);
			memcpy(b2->getData(), b->getData(), b->getSize());
			b2->setSize(b->getSize());

			Engine::shared()->memoryManager.destroyBuffer( b );
			
			return b2;
			
		}
		
		static Buffer* decompressBuffer(Buffer *buffer)
		{
			return decompressBuffer( buffer->getData() , buffer->getSize() , buffer->getType() );
		}
		
		
		static Buffer* decompressBuffer( char * data , size_t length , Buffer::BufferType type )
		{
			// Get the buffer with the rigth size
			LM_TODO(("Review the decompression process estimating the original size on the fly"));
			assert( false );	
			
			size_t compressed_size = length;
			
			long int m2_len = 0; //?
			Buffer *b =  Engine::shared()->memoryManager.newBuffer( "Decompressed buffer" , m2_len , type );
			b->setSize(m2_len);
			
			// Decompress information
			int ans_decompress = ezuncompress( (unsigned char*) b->getData(), &m2_len, (unsigned char*) ( data  ) , compressed_size );	

			if (ans_decompress != 0)
				LM_X(1, ("ezuncompress error"));
			
			
			return b;
			
		}

		
		
	};
}

#endif
