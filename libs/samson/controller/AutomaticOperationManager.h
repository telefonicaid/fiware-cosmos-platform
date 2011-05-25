#ifndef _H_AUTOMATIC_OPERATION_MANAGER
#define _H_AUTOMATIC_OPERATION_MANAGER

#include "samson/common/coding.h"						// samson::KVInfo
#include "MonitorParameter.h"			// samson::MonitorBlock
#include <list>							// std::list
#include "samson/module/KVFormat.h"			// KVFormat
#include "samson/common/samson.pb.h"					// samson::network::...
#include <vector>						// std::vector
#include <iostream>						// std::cout
#include "au/Lock.h"						// au::Lock


namespace samson {

	class AutomaticOperation;
	
	struct AOInfo
	{
		size_t id;						// Id of the automatic operation
		std::string command;			// Command that we need to run
	};
	
	class AutomaticOperationManager
	{
		
		au::map<size_t , AutomaticOperation > operations;
		size_t counter;
		
		
	public:
		
		AutomaticOperationManager();
		
		void add( AutomaticOperation* ao );

		void remove( size_t id );
		
		void removeAllWithTag( std::string tag );
		
		// Get the next automatic operation
		std::vector<AOInfo> getNextAutomaticOperations();

		// Set as finished an automatic operation
		void finishAutomaticOperation( size_t id ,bool error , std::string error_message );
		
		// Fill information
		void fill( network::AutomaticOperationList *aol , std::string command);

		// Clear everything
		void clear();
		
	};
	
}


#endif