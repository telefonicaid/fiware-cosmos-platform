
#include "AutomaticOperation.h"			// Own interface
#include "Queue.h"						// ss::Queue
#include "au/Format.h"						// au::Format

namespace ss {

	#pragma mark AOQueueThrigger

	std::string AOQueueThrigger::str()
	{
		std::ostringstream o;
		o << "[Q:" << queue->name() << " " << au::Format::string( min_size ) << " " << au::Format::string( min_num_kvs ) << "]";
		return o.str();
	}
	
	
	bool AOQueueThrigger::thrigger()
	{
		
		FullKVInfo info = queue->info();
		
		if( info.isEmpty() )	// If no content, never thrigger...
			return false;
		
		if( ( min_num_kvs > 0 ) && (info.kvs < min_num_kvs ))
			return false;
		
		if( ( min_size > 0 ) && (info.size < min_size ))
			return false;
		
		return true;
	}
	
	


}