#include <assert.h>

#include "KVSet.h"
#include "TaskManager.h"
#include "TaskBase.h"            /* Own interface                   */



namespace ss {

	class Task;

	TaskBase::TaskBase( size_t id , Task * parentTask  )
	{
		status = definition;	// Initial status of definition of the task

		_parentTask = parentTask;
		_id = id;	
		
		numberChildren = 0;	// No children needed to wait
	}
	

	Task * TaskBase::getParentTask(  )
	{
		return _parentTask;
	}
	
	size_t TaskBase::getId()
	{
		return _id;
	}
	
	
	void TaskBase::addKVSets( KVSetVector* _sets )
	{
		assert( status == definition );
		sets.insert( sets.end() , _sets->begin() , _sets->end() );
	}
	
	
}



