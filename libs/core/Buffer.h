#ifndef SAMSON_BUFFER_H
#define SAMSON_BUFFER_H

#include <stdio.h>

#include "AUCommandLine.h"
#include "AUConsole.h"
#include "Format.h"
#include "Factory.h"
#include "common.h"
#include "Sockets.h"



namespace ss {

/**
 
 Simple buffer 
 
 */
	
class Buffer
{
	
	char * _buffer;		//!< Internal buffer to store key-value information
	size_t _size;		//!< Current size in the buffer
	size_t _max_size;	//!< Max length in the buffer (allocated in memory)
	

public:
	Buffer()
	{
		_max_size = 0;
		_size = 0;
		_buffer = NULL;
	}

	
	Buffer(size_t memory)
	{
		_max_size = 0;
		_size = 0;
		_buffer = NULL;
		
		initBufferWithMemory(memory);
	}
	
	~Buffer()
	{
		freeBuffer();
	}
	
	void initBufferWithMemory( size_t size )
	{
		freeBuffer();	//Make sure it is released
		
		_max_size = size;
		_size = 0;
		
		//Alloc space for this buffer
		_buffer = (char*) malloc( _max_size );
	}	
	
	bool checkEnougthSpace( size_t size )
	{
		return ( (_size + size) <= _max_size );
	}
	
	size_t get_max_size()
	{
		return _max_size;
	}
	
	size_t get_size()
	{
		return _size;
	}
	
	char * getBufferPointer()
	{
		return _buffer;
	}

	
	void reallocToSize( size_t new_size )
	{
		_buffer = (char*)realloc( _buffer , new_size );
		_max_size = new_size;
		assert( _buffer );		//We assume there are no errors here
	}
	
	void write( char * buffer  , size_t size)
	{
		assert( _buffer );
		assert( _size + size < _max_size );
		memcpy(_buffer + _size, buffer, size);
		_size += size;
	}
	
	void clear()
	{
		_size = 0;
	}
	
	
	void freeBuffer()
	{
		if( _buffer )
		{
			//fprintf(stderr, "Freeing Buffer %lu bytes\n", memory);
			free(_buffer);
			_size = 0;
			_max_size = 0;
			_buffer = NULL;
		}
	}
	
	
};
}

#endif
