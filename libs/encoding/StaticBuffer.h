

#pragma once


/** Simple static Buffer to hold information */

namespace ss {

	class StaticBuffer
	{
		char *_data;
		size_t _length;
		
	public:
		
		StaticBuffer()
		{
			_data = NULL;
			_length = 0;
		}
		
		void initStaticBuffer (char *data , size_t length )
		{
			_data = data;
			_length = length;
		}
		
		char* getData()
		{
			return _data;
		}
		
		size_t getLength()
		{
			return _length;
		}
		
		
	};

}