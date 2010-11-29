
#ifndef _H_WORKER_TASK
#define _H_WORKER_TASK

#include "au_map.h"				// au::map
#include <cstring>				// size_t
#include <string>				// std::string
#include "samson/Operation.h"	// ss::Operation
#include "samson.pb.h"			// ss::network::...
#include "samson/Environment.h"	// ss::Environment


namespace ss {

	
	class WorkerTaskItem;
	
	class WorkerTask
	{
		
	public:
		
		// identifier of the task
		size_t task_id;
		
		// Number of items ( used as a counter to add new items )
		int num_items;	
		
		// Error management
		bool error;
		std::string error_message;
		
		std::string operation;					// Operation to be executed
		au::map<int,WorkerTaskItem> item;		// List of items to be executed by processes
		
		network::Environment environment;		// Environment for this task ( transmitted to all process )
		
		WorkerTask( Operation::Type type , const network::WorkerTask &task );
		
		// Get the next item ( if any )
		WorkerTaskItem *getNextItemToProcess();

		// Notify that an item is done
		void finishItem( size_t id , bool _error , std::string _error_message );
		
		// Getting info
		std::string getStatus();
		bool isFinish();
		size_t getId();
		
	private:
		void addItem( WorkerTaskItem *item );

		
	};
}
#endif
