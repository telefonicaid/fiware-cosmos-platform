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
		// Global parameters identifying this buffer
		int _sharedMemoryId;		// SharedObject (-1 if not shared memory)

		// Buffer itself
		char * _data;
		size_t _size;

		// Private constructor/destructors since it can be only access by MemoryManager
		friend class MemoryManager;
		
		Buffer( int sharedMemoryId , char *data ,  size_t size )
		{
			_sharedMemoryId = sharedMemoryId;
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

		size_t offset;
			
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
