
#ifndef _H_DATA_QUEUE
#define _H_DATA_QUEUE

#include <string>						// std::string
#include "MonitorParameter.h"			// ss::MonitorBlock
#include <cstring>						// size_t

namespace ss {

	class DataQueue
	{
		std::string name;				// Name of the queue
		size_t size;					// Total Size of this queue
		
		MonitorBlock monitor;			// Set of parameters to monitor for this queue
		
		friend class Monitor;
		
	public:
		
		DataQueue( std::string _name)
		{
			name = _name;
			size = 0 ;
		}
		
		void addFile( int worker, std::string _fileName , size_t _size )
		{
			size += _size;
		}
		
		size_t getSize()
		{
			return size;
		}
		
		std::string getName()
		{
			return name;
		}
		
		
	};
	
}

#endif
