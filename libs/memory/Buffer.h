#ifndef _H_BUFFER
#define _H_BUFFER

#include <cstring>			// size_t
#include <cstdlib>			// malloc, ...
#include <fstream>			// std::ifstream

namespace ss {

	/**
	 Buffer class to hold data manager by MemoryManager
	 It has to be accessible by multiple process identified by offset and size
	 */
	
	class Buffer
	{

		char * _data;			// Buffer of data
		size_t _max_size;		// Maximum size of this buffer

		
		/**
		 Current size used in this buffer 
		 This is the one that should be used when transmitting the buffer across the network
		 This variable is updated with calls to "write"
		 */
		
		size_t _size;			

		/**
		 Internal variable used for the read process
		 */
		
		size_t _offset;
		
	private:
		
		// Private constructor/destructors since it can be only access by MemoryManager
		friend class MemoryManager;
		
		Buffer( size_t max_size )
		{
			_max_size = max_size;
			_offset = 0;

			if( max_size > 0)
				_data = (char *) malloc(max_size);
			else
				_data = NULL;
			
			_size = 0;
		}
		
		~Buffer()
		{
			free();
		}
		
		void free()
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
		
	public:
		
		/**
		 Function to write content updating the size variable coherently
		 If it is not possible to write the entire block, it return false
		 So, it never try to write less than input_size bytes
		 */
		
		bool write( char * input_buffer , size_t input_size )
		{
			if( _size + input_size > _max_size )
				return false;
			
			memcpy(_data + _size, input_buffer, input_size);
			_size += input_size;
			
			return true;
		}
		
		
		bool skipWrite( size_t size )
		{
			if( _size + size <= _max_size)
			{
				_size += size;
				return true;
			}
			else
				return false;
		}
			
		
		size_t skipRead( size_t size)
		{
			if( _offset + size > _max_size)
				size = (_max_size - _offset);	// Max offset
				
			_offset += size;
			return size;
						
		}
		
		/**
		 Write on the buffer the maximum possible ammount of data
		 */
		
		void write( std::ifstream &inputStream )
		{
			inputStream.read( _data + _size , _max_size - _size ); 
			_size += inputStream.gcount();
		}
		

		/* 
		 Remove the last characters of an unfinished line and put them in buffer.
		 Remove the size of this set of characters
		 */
		
		size_t removeLastUnfinishedLine( char * buffer)
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
		
		size_t read( char *output_buffer, size_t output_size)
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
		
		size_t getSizePendingRead()
		{
			return _size - _offset;
		}
		
		size_t getAvailableWrite()
		{
			return _max_size - _size;
		}
		
		char *getData()
		{
			return _data;
		}
		
		size_t getMaxSize()
		{
			return _max_size;
		}
		
		size_t getSize()
		{
			return _size;
		}
		
		void setSize( size_t size )
		{
			if (( size >= 0) && (size <= _max_size))
				_size = size;
		}
		
		
		

	};

}

#endif
