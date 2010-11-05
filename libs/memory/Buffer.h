#ifndef _H_BUFFER
#define _H_BUFFER

#include <cstring>			// size_t
#include <cstdlib>			// malloc, ...

namespace ss {

	/**
	 Buffer class to hold data manager by MemoryManager
	 It has to be accessible by multiple process identified by offset and size
	 */
	
	class Buffer
	{

		// Buffer itself
		char * _data;
		size_t _size;

		// Private constructor/destructors since it can be only access by MemoryManager
		friend class MemoryManager;
		
		Buffer( char *data ,  size_t size )
		{
			_size = size;
			_data = data;

			offset = 0;
		}
		
		~Buffer()
		{
		}
		
		void free()
		{
			if( _data )
				::free( _data );
			_size = 0;
		}
		
	public:

		/** 
		 Variables used by the owner of this buffer
		 */
		
		size_t offset;
			
		
		char * getCurrentDataAndUpdateOffset( size_t size )
		{
			offset += size;
			return _data + offset - size; 
		}
		
	public:
		
		char *getData()
		{
			return _data;
		}
		
		size_t getSize()
		{
			return _size;
		}
	};

}

#endif
