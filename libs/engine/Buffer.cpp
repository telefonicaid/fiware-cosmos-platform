/* ****************************************************************************
 *
 * FILE                     Buffer.h
 *
 * AUTHOR                   Andreu Urruela
 *
 * CREATION DATE            2010
 *
 */

#include <sstream>          // std::stringstream
#include "au/Format.h"		// au::Format
#include "engine/Buffer.h"	// Own interface

namespace engine {

	Buffer::Buffer( std::string name ,   size_t max_size , int _tag )
	{
		_max_size = max_size;
		_offset = 0;
		
		tag = _tag;
		
		_name = name;
		
		if( max_size > 0)
		{
			_data = (char *) malloc(max_size);
			if( !_data )
			{
#ifdef __LP64__
			   fprintf(stderr,"Error allocating memory for %lu bytes", max_size);
#else
			   fprintf(stderr,"Error allocating memory for %d bytes", max_size);
#endif
			   exit(1);
			}
		}
		else
			_data = NULL;
		
		_size = 0;
	}
	
	Buffer::~Buffer()
	{
		free();
	}
	
	std::string Buffer::str()
	{
		std::ostringstream output;
		output << "[ Buffer size:" << au::Format::string( _size ) << " max:" << au::Format::string( _max_size ) ;
		return output.str();
	}

	
	void Buffer::free()
	{
		if( _data )
		{
			::free( _data );
			_data = NULL;
		}
		_max_size = 0;
		_size = 0;
		_offset = 0;
	}
	
	/**
	 Function to write content updating the size variable coherently
	 If it is not possible to write the entire block, it return false
	 So, it never try to write less than input_size bytes
	 */
	
	bool Buffer::write( char * input_buffer , size_t input_size )
	{
		if( _size + input_size > _max_size )
			return false;
		
		memcpy(_data + _size, input_buffer, input_size);
		_size += input_size;
		
		return true;
	}
	
	
	bool Buffer::skipWrite( size_t size )
	{
		if( _size + size <= _max_size)
		{
			_size += size;
			return true;
		}
		else
			return false;
	}
	
	
	size_t Buffer::skipRead( size_t size)
	{
		if( _offset + size > _max_size)
			size = (_max_size - _offset);	// Max offset
		
		_offset += size;
		return size;
		
	}
	
	/**
	 Write on the buffer the maximum possible ammount of data
	 */
	
	void Buffer::write( std::ifstream &inputStream )
	{
		inputStream.read( _data + _size , _max_size - _size ); 
		_size += inputStream.gcount();
	}
	
	
	/* 
	 Remove the last characters of an unfinished line and put them in buffer.
	 Remove the size of this set of characters
	 */
	
	size_t Buffer::removeLastUnfinishedLine( char * buffer)
	{
		size_t last_line_size = 0;
		while( _data[_size - last_line_size - 1] != '\n')
			last_line_size++;
		
		memcpy(buffer, _data + _size - last_line_size , last_line_size);
		
		_size -= last_line_size;
		return last_line_size;
	}
	
	/**
	 Read content of the buffer in a continuous way
	 */
	
	size_t Buffer::read( char *output_buffer, size_t output_size)
	{
		size_t read_size = output_size;
		if( read_size > ( _size - _offset ) )
			read_size = (_size - _offset);
		
		memcpy(output_buffer, _data+_offset, read_size);
		_offset += read_size;
		return read_size;
	}
	
	/**
	 Auxilir functions to work directly with the content
	 */
	
	size_t Buffer::getSizePendingRead()
	{
		return _size - _offset;
	}
	
	size_t Buffer::getAvailableWrite()
	{
		return _max_size - _size;
	}
	
	char *Buffer::getData()
	{
		return _data;
	}
	
	size_t Buffer::getMaxSize()
	{
		return _max_size;
	}
	
	size_t Buffer::getSize()
	{
		return _size;
	}
	
	void Buffer::setSize( size_t size )
	{
		if ( size <= _max_size )
			_size = size;
	}
	
	
	SimpleBuffer Buffer::getSimpleBuffer()
	{
		return SimpleBuffer( _data , _max_size );
	}
	
	SimpleBuffer Buffer::getSimpleBufferAtOffset(size_t offset)
	{
		return SimpleBuffer( _data + offset , _max_size - offset );
	}
	

	
}
