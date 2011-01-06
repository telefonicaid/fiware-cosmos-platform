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

		

		static Buffer* compressBuffer( Buffer *buffer )
		{
			
			// Check input buffer format
			BufferHeader *_header = (BufferHeader*) buffer->getData();
			assert( _header->check() );
			assert( _header->compressed == 0);
			
			// Create the header for the output buffer
			BufferHeader header;						// Header to write as header output
			header.init( 1 );							// Init header ( and magic number )

			// Get the original size
			header.original_size = buffer->getSize() - sizeof(BufferHeader);	// Set the original size
			
			// Destination buffer
			long int cm_max_len = EZ_COMPRESSMAXDESTLENGTH( header.original_size );
			long int cm_len = cm_max_len;

			// Get a new buffer ( a little bit larger )
			Buffer *b = MemoryManager::shared()->newBuffer( "Compressed buffer", cm_len + sizeof(BufferHeader)  );
			
			int ans_compress = ezcompress( ( unsigned char*) ( b->getData() + sizeof(BufferHeader) ), &cm_len, ( unsigned char*) (buffer->getData() + sizeof(BufferHeader)) , header.original_size );
			assert( !ans_compress );

			header.compressed_size = cm_len; 
			
			memcpy(b->getData(),(char*) &header, sizeof(BufferHeader));
			
			b->setSize(cm_len + sizeof( BufferHeader ) );			// Set the size of the compressed buffer

			return b;
		}
		
		static Buffer* decompressBuffer(Buffer *buffer)
		{
			return decompressBuffer( buffer->getData() , buffer->getSize() );
		}
		
		
		static Buffer* decompressBuffer( char * data , size_t length )
		{
			// Decompress
			BufferHeader *header = (BufferHeader*) data;
			assert( header->check() );
			assert( header->compressed_size == (length - sizeof(BufferHeader)) );

			assert( header->compressed == 1 );	// Otherwise this function should not be called
			
			// Get the buffer with the rigth size
			long int m2_len = header->original_size;	
			Buffer *b =  MemoryManager::shared()->newBuffer( "Decompressed buffer" , m2_len );
			b->setSize(m2_len);
			
			// Decompress information
			int ans_decompress = ezuncompress( (unsigned char*) b->getData(), &m2_len, (unsigned char*) ( data + sizeof( BufferHeader ) ) , header->compressed_size );	
			assert(!ans_decompress);
			
			assert( (size_t)m2_len == header->original_size );
			
			return b;
			
		}

		
		
		~Packet()
		{
		}
	};
}

#endif
