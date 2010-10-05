#include "MemoryController.h"
#include "Task.h"
#include "KVSet.h"
#include "SSMonitor.h"
#include "KVManager.h"
#include "SSLogger.h"
#include "au.h"

namespace ss {

	bool MemoryController::alloc( size_t size )
	{
		if ( _used_memory + size <= _memory )
		{
			_used_memory += size;
			return true;
		}
		return false;
	}
	
	void MemoryController::dealloc(size_t size )
	{
		_used_memory -= size;
	}
	
	void MemoryController::addAvailableMemory( size_t memory )
	{
		_memory += memory;
	}

	bool MemoryController::isMemoryUsageHigh()
	{
		return ( ( (double) _used_memory  / (double) _memory  ) > 0.8 );
	}
	
	std::string MemoryController::str()
	{
		std::ostringstream o;
		o << "Memory: " << au::Format::string( _memory );
		o << " Used: " << au::Format::string( _used_memory );
		o << std::endl;
		return o.str();
	}
		
	
	size_t MemoryController::availableMemory()
	{
		return ( _memory - _used_memory);
	}
	
}

