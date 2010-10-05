#ifndef SAMSON_MEMORY_CONTROLLER_H
#define SAMSON_MEMORY_CONTROLLER_H

#include <sstream>
#include <cstring>



/**
 TODO: Create a thread for each disk (optimize input / output)
 */

namespace ss {

	
	class MemoryController
	{
		
		size_t _memory;
		size_t _used_memory;

	public:
		
		MemoryController()
		{
			_memory = 0;
			_used_memory = 0;
		}			
		
		void addAvailableMemory( size_t memory );
		bool alloc(size_t size );
		void dealloc( size_t size );
		bool isMemoryUsageHigh();
		size_t availableMemory();
		std::string str();
		
	};
}

#endif
