

#include "WorkerTask.h"			// OwnInterface
#include "WorkerTaskItem.h"		// ss::WorkerTaskItem

#include "WorkerTaskItemParser.h"		// ss::WorkerTaskItemParser
#include "WorkerTaskItemGenerator.h"	// ss::WorkerTaskItemGenerator
#include "WorkerTaskManager.h"			// ss::WorkerTaskManager


namespace ss
{
	
	WorkerTask::WorkerTask(WorkerTaskManager *_taskManager , Operation::Type type , const network::WorkerTask &task )
	{
		taskManager = _taskManager;		// Pointer to the task manager
		
		operation = task.operation();	// Save the operation to perform		
		task_id = task.task_id();		// Save the task id

		reduceInformation = NULL;	// By default this is not used
		
		num_items = 0;
		
		// copy all the environment variables
		environment = task.environment();
		
		// By default no error
		error = false;
		
		if( type == Operation::generator )
		{
			// In generators only one of the workers is active to create key-values
			if( task.generator() )
			{
				addItem( new WorkerTaskItemGenerator( task ) );
			}
			
			return;
		}
		
		if( type == Operation::parser )
		{
			// An item per file
			assert( task.input_size() == 1);	// Only one input
			network::FileList fl = task.input(0);
			
			for (size_t i = 0 ; i < (size_t) fl.file_size() ; i++)
				addItem( new WorkerTaskItemOperation( fl.file(i).name() , task ) );
			
			return;
		}

		if( type == Operation::map )
		{
			// An item per file
			assert( task.input_size() == 1);	// Only one input
			network::FileList fl = task.input(0);
			
			for (size_t i = 0 ; i < (size_t) fl.file_size() ; i++)
				addItem( new WorkerTaskItemOperation( fl.file(i).name() , task ) );

			return;
		}

		if( type == Operation::reduce )
		{
			// Just one item to organize all the reduce "sub operations"
			addItem( new WorkerTaskItemReduceOrganizer( this, task ) );
			return;
		}
		
		
		
	}
	
	WorkerTask::~WorkerTask()
	{
		if( reduceInformation )
			delete reduceInformation;
	}
	
	
	void WorkerTask::addItem( WorkerTaskItem * i )
	{
		int itemId = num_items++;
		
		i->setTaskAndItemId(this , itemId);
		item.insertInMap( itemId , i  );
		
	}
	
	WorkerTaskItem *WorkerTask::getNextItemToProcess()
	{
		std::map<int,WorkerTaskItem*>::iterator iterator;
		for (iterator = item.begin() ; iterator != item.end() ; iterator++)
		{
			WorkerTaskItem *item = iterator->second;
			if( item->isReadyToRun() )
				return item;
		}
		
		return NULL;
	}
		
	bool WorkerTask::isFinish()
	{
		return ( item.size() == 0);	// No more items to process
	}
	
	std::string WorkerTask::getStatus()
	{
		std::ostringstream output;
		output << "\tTask " << task_id << " Operation: " << operation;
		if( isFinish() )
			output << " [FINISH] ";
		output << std::endl;
		
		output << getStatusFromArray( item );
		return output.str();
	}
	
	size_t WorkerTask::getId()
	{
		return task_id;
	}
	
	void WorkerTask::finishItem( size_t id , bool _error , std::string _error_message )
	{
		
		WorkerTaskItem *i = item.extractFromMap( id );
		i->freeResources();
		
		delete i;
		
		if( _error )
		{
			error = true;
			error_message = _error_message;
		}
		
	}	
}