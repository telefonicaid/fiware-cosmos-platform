#ifndef SAMSON_TASK_BASE_H
#define SAMSON_TASK_BASE_H

#include "KVSetVector.h"         /* KVSetVector                     */


/**
 Basic task for all the platform
 The only required is the implementation of the "run" function.
 */

namespace ss {

	class KVSet;
	class Task;
	
	enum TaskStatus
	{
		definition,
		in_queue,
		running,
		waiting,
		finished
	};


	/**
	 Base class of Task.
	 It gives the functionality to add KVSet / KVStorage dependencies 
	 */
	
	class TaskBase 
	{
		friend class KVManager;
		friend class KVSetsManager;

		// Necessary KVSets in memory ( sorted by the queue they belong )
		std::vector<KVSet*>  sets;				//!< All the KVSets that should be in memory for reading
		Task*                _parentTask;		//!< Parent to report when finished (reducing the number of children)
		size_t               _id;				//!< Identifier of the task

		/**
		 Add a KVSet that should be on memory during execution
		 */
		
		void addKVSets(KVSetVector *sets);
		
	public:		
		TaskStatus status;				// Status of the task
		int        numberChildren;	    // Number of children that should report finished (protected by the lock of TaskManager)
		
		TaskBase(size_t id, Task* parentTask);

		Task*   getParentTask();
		size_t  getId();
	};
}

#endif
