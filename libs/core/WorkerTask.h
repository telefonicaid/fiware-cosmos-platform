
#ifndef _H_WORKER_TASK
#define _H_WORKER_TASK

#include "au_map.h"				// au::map
#include <cstring>				// size_t
#include <string>				// std::string
#include "samson/Operation.h"	// ss::Operation
#include "samson.pb.h"			// ss::network::...
#include "samson/Environment.h"	// ss::Environment
#include "Status.h"				// au::Status

namespace ss {

	
	class WorkerTaskItem;
	class WorkerTaskManager;
	class ProcessAssistantSharedFileCollection;
	
	class WorkerTask : public au::Status
	{
		
	public:

		// Pointer to the task manager
		WorkerTaskManager *taskManager;
		
		// identifier of the task
		size_t task_id;
		
		// Number of items ( used as a counter to add new items )
		int num_items;
		int num_finish_items;
		
		// Error management
		bool error;
		std::string error_message;
		
		std::string operation;					// Operation to be executed
		au::map<int,WorkerTaskItem> item;		// List of items to be executed by processes
		
		WorkerTask(WorkerTaskManager *taskManager, Operation::Type type , const network::WorkerTask &task );
		~WorkerTask();
		
		WorkerTaskItem *getNextItemToProcess();			// Get the next item ( if any )

		void setup();	// Setup all internal tasks
		
		// Notify that an item is done ( by ProcessAssistant )
		void finishItem( size_t id );
		
		// Getting info
		bool isFinish();
		size_t getId();

		// Funciton to get the run-time status of this object
		void getStatus( std::ostream &output , std::string prefix_per_line );
		
		ProcessAssistantSharedFileCollection *reduceInformation;		// Structure used only by reduce operations ( created with the orgnizer and used by all process assistants )
		
	private:
		
		friend class WorkerTaskItemOrganizer;	// to be able to add individual reduce items
		
		void addItem( WorkerTaskItem *item );

		void setError(std::string _error_message)
		{
			error = true;
			error_message = _error_message;
		}
		
		
	};
}
#endif
