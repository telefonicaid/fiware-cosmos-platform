#ifndef SAMSON_KV_SET_BUFFER_H
#define SAMSON_KV_SET_BUFFER_H

#include <queue>
#include <set>
#include <map>
#include <iostream>
#include <assert.h>
#include <pthread.h>
#include <string>

#include "BufferSizes.h"         /* MAX_SIZE_KEY_VALUE                       */
#include "samson.pb.h"
#include "KVFormat.h"



namespace ss {
	class KVManager;
	class DataInstance;
	
	/**
	 KVSetBuffer: 
	 Buffer used to hold data on memory while producing data in a particular task
	 */
	 
	typedef struct{
		char *buffer;
		size_t size;
	} DataBuffer;
	
	class KVSetBufferBase
	{
	public:
		virtual DataBuffer getDataBuffer()=0;
	};

	/**
	 Standart KVSetBuffer
	 */
	
	class KVSetBuffer : public KVSetBufferBase
	{
		// Counter of kvs
		size_t _num_kvs;			//!< Number of kvs inside this set (total on disk + memory)

		KVFormat _format;			//!< Format of the kvs
		
		// Internal buffer
		char * _buffer;				//!< Internal buffer to store key-value information
		size_t _buffer_size;		//!< Current size in the buffer
		size_t _buffer_max_size;	//!< Max length in the buffer (allocated in memory)
		
		//Intermediate buffers to serialize keys / values / total
		char *buffer;
		char *buffer_key;
		char *buffer_value;
		
		bool closed;					//!< Flag to indicate that this set is closed ( necessary to be transformed into a KVSet )	
		
	public:
		
		KVSetBuffer( KVFormat format )
		{
			_format = format;
			
			// Number of KVs
			_num_kvs = 0;
			
			buffer_key = (char*) malloc( MAX_SIZE_KEY_VALUE );
			buffer_value = (char*) malloc( MAX_SIZE_KEY_VALUE );
			buffer = (char*) malloc( 32 + 2*MAX_SIZE_KEY_VALUE );	// Taking into account the size of "size"
			
			// Internal buffer
			_buffer = NULL;
			_buffer_size = 0;
			_buffer_max_size = 0;

			// Mark as "non closed"
			closed = false;
		}
	
		DataBuffer getDataBuffer()
		{
			DataBuffer b;
			b.buffer = _buffer;
			b.size = _buffer_size;
			return b;
		}

		
		~KVSetBuffer()
		{
			free(buffer);
			free(buffer_key);
			free(buffer_value);
			
		}
	
		size_t getNumKVs()
		{
			return _num_kvs;
		}

		/**
		 Get the blocal size of this buffer
		 */		
		
	private:
		void write( char * buffer , size_t size );
		
	public:
		
		void reserveSpace( size_t size );
		
		void emit( DataInstance * key , DataInstance * value );
		void close( );
		bool isClosed();
		
		size_t maxSizeForSize( size_t size );
		
	};
	
	
	/**
	 Spetial KVSetBuffer for txt sets ( from network connections )
	 */

	class KVSetTxTBuffer : public KVSetBufferBase
	{
		
		char *txtBuffer;
		size_t txtBufferSize;
		
		bool closed;
		
		DataBuffer dataBuffer;
	public:		
		
		KVSetTxTBuffer( size_t size )
		{
			txtBuffer = (char *) malloc( size );
			txtBufferSize = size;
			closed = false;
		}

		char *getTxtDataBuffer()
		{
			return txtBuffer;
		}
		
		DataBuffer getDataBuffer()
		{
			assert( closed );
			return dataBuffer;
		}

		void close()
		{
			assert( !closed );
			
			// Counter the number of lines ( as kvs )
			size_t num_lines = 0 ;
			for (size_t i = 0 ; i < txtBufferSize ; i++)
				if( ( txtBuffer[i] == '\n' ) || ( txtBuffer[i] == '\0' ) )
				{
					txtBuffer[i] = '\0';	// Unified criteria of "new line"
					num_lines++;
				}
			
			// Prepare the packet
			protocol::KVSetHeader header;
			header.set_size( txtBufferSize );
			header.set_num_kvs( num_lines );
			header.set_txtformat( true );
			header.set_hashformat( false );
			header.mutable_format()->set_key( "system.String" );
			header.mutable_format()->set_value( "system.Void" );
			
			size_t header_size = header.ByteSize();
			size_t total_size =  sizeof(size_t) + header_size + txtBufferSize;
			
			dataBuffer.buffer = (char*) malloc( total_size );
			dataBuffer.size = total_size;
			
			// Write the header size
			*((size_t*)dataBuffer.buffer) = header_size;
			
			// Write the header itself
			char header_buffer[10000];
			header.SerializeToArray(header_buffer, header_size);
			memcpy(dataBuffer.buffer + sizeof(size_t), header_buffer, header_size);
			
			// Write the buffer itself
			memcpy(dataBuffer.buffer + sizeof(size_t) + header_size, txtBuffer, txtBufferSize );

			free( txtBuffer );
			txtBuffer = NULL;
			
			
			closed = true;
		}
	};
}

#endif
