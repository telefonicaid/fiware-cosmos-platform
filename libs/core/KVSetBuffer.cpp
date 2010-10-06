#include "KVSetBuffer.h"
#include "KVManager.h"
#include "MemoryController.h"
#include "Data.h"



/**
 Function to compute the space necessary to be allocated
 */

namespace ss
{

	size_t KVSetBuffer::maxSizeForSize( size_t size )
	{
		if( size == 0)
			return 0;

		size_t new_buffer_max = 64*1024;	// Initial size 64Kb
		while( new_buffer_max < size )
			new_buffer_max = 2*new_buffer_max;
		return new_buffer_max;
	}



	void KVSetBuffer::write( char * buffer , size_t size )
	{
		assert(!closed);
		if( size == 0)
			return;

		reserveSpace(size);

		// Write to the buffer
		memcpy(_buffer + _buffer_size, buffer, size);
		_buffer_size += size;
	}
 
	
	bool KVSetBuffer::isClosed()
	{
		return closed;
	}
	
	
	void KVSetBuffer::reserveSpace( size_t size )
	{
		assert(!closed);
		
		size_t _new_buffer_size = _buffer_size + size;
		
		// Check if there is space in the buffer
		if( _new_buffer_size > _buffer_max_size )
		{
			// New max size for the buffer
			size_t _new_buffer_max_size = maxSizeForSize( _new_buffer_size );
			
			// Alloc memory controlled by the memory controller
			KVManager::shared()->alloc( _new_buffer_max_size - _buffer_max_size );
			
			// Realloc the memory buffer to acomodate more space
			char *_new_buffer = (char*) malloc( _new_buffer_max_size );
			memcpy(_new_buffer, _buffer, _buffer_size );
			::free( _buffer );
			
			_buffer = _new_buffer;
			_buffer_max_size = _new_buffer_max_size;	
		}
		
	}
	

	
#pragma mark KVSetBuffer
	
	void KVSetBuffer::emit( DataInstance * key , DataInstance * value )
	{
		size_t key_size	= key->serialize( buffer_key  );
		size_t value_size = value->serialize( buffer_value );
		
		char * _buffer = buffer;
		size_t key_size_offset = staticVarIntSerialize(_buffer, key_size);
		_buffer += key_size_offset;
		
		memcpy(_buffer, buffer_key, key_size);
		_buffer+=key_size;
		
		size_t value_size_offset = staticVarIntSerialize(_buffer, value_size);
		_buffer += value_size_offset;
		
		memcpy(_buffer, buffer_value, value_size);
		_buffer+=value_size;
		
		size_t size = key_size_offset + key_size + value_size_offset + value_size;
		
		// Write the final buffer and size to the buffer
		write( buffer ,size);
		
		_num_kvs++;
	}
	
	void KVSetBuffer::close()
	{
		assert(!closed);	// You can only close once
		
		size_t free_memory = _buffer_max_size - _buffer_size;
		
		
		// Create the KVSetHeader message
		protocol::KVSetHeader header;
		header.set_size( _buffer_size );
		header.set_num_kvs( _num_kvs );
		header.set_txtformat( false );
		header.set_hashformat( false );
		header.mutable_format()->set_key( _format.keyFormat );
		header.mutable_format()->set_value( _format.valueFormat );
		
		size_t header_size = header.ByteSize();
		size_t total_size = _buffer_size + sizeof(size_t) + header_size;
		
		char *tmp = (char*) malloc( total_size );
		
		// Write the header size
		*((size_t*)tmp) = header_size;
		
		// Write the header itself
		char header_buffer[10000];
		header.SerializeToArray(header_buffer, header_size);
		memcpy(tmp + sizeof(size_t), header_buffer, header_size);
		
		// Write the buffer itself
		memcpy(tmp + sizeof(size_t) + header_size, _buffer, _buffer_size);
		free( _buffer );
		
		_buffer = tmp;
		_buffer_size = total_size;
		_buffer_max_size = total_size;
		
		closed = true;
		
		// Alloc memory controlled by the memory controller
		KVManager::shared()->dealloc( free_memory );
		
	}
	
	
}



