#ifndef _H_AUTOMATIC_OPERATION_MANAGER
#define _H_AUTOMATIC_OPERATION_MANAGER

#include "coding.h"						// ss::KVInfo
#include "MonitorParameter.h"			// ss::MonitorBlock
#include <list>							// std::list
#include "samson/KVFormat.h"			// KVFormat
#include "samson.pb.h"					// ss::network::...
#include <vector>						// std::vector
#include <iostream>						// std::cout
#include "Lock.h"						// au::Lock


namespace ss {

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

		
	};
	
}


#endif