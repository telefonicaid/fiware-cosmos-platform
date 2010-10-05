#ifndef SAMSON_TASK_H
#define SAMSON_TASK_H

#include <list>
#include <vector>
#include <cstdlib>
#include <string>
#include <sstream>
#include <assert.h>
#include <iostream>
#include <deque>
#include <set>
#include <map>

#include "au.h"
#include "KVQueue.h"



/**
 Basic task for all the platform
 The only required is the implementation of the "run" function.
 */

namespace ss {

	class TaskManager;
	class KVSet;
	class KVStorage;
	class Task;
	class KVQueue;	
	
	enum TaskStatus{ definition , in_queue, running , waiting,  finished };
	
	/**
	 Base class of Task.
	 It gives the functionality to add KVSet / KVStorage dependencies 
	 */
	
	class TaskBase 
	{
		
		// Necessary KVSets in memory ( sorted by the queue they belong )
		
		std::vector<KVSet*> sets;				//!< All the KVSets that should be in memory for reading
		Task* _parentTask;						//!< Parent to report when finished (reducing the number of children)
		size_t _id;								//!< Identifier of the task

		friend class KVManager;
		friend class KVSetsManager;

		/**
		 Add a KVSet that should be on memory during execution
		 */
		
		void addKVSets( KVSetVector *sets );
		
	public:
		
		TaskStatus status;						// Status of the task
		int numberChildren;		 			    // Number of children that should report finished (protected by the lock of TaskManager)
		
		TaskBase( size_t id , Task * parentTask  );

		/**
		 Get information about the id and parent task
		 */
		
		Task* getParentTask(  );
		size_t getId();

		
		
	};
	
}

#endif
