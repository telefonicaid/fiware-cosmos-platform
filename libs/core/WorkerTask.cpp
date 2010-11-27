

#include "WorkerTask.h"			// OwnInterface
#include "WorkerTaskItem.h"		// ss::WorkerTaskItem

#include "WorkerTaskItemParser.h"		// ss::WorkerTaskItemParser
#include "WorkerTaskItemGenerator.h"	// ss::WorkerTaskItemGenerator

namespace ss
{
	
	WorkerTask::WorkerTask( Operation::Type type , const network::WorkerTask &task )
	{
		operation = task.operation();	// Save the operation to perform		
		task_id = task.task_id();		// Save the task id
		
		// By default no error
		error = false;
		
		if( type == Operation::generator )
		{
			// In generators only one of the workers is active to create key-values
			if( task.generator() )
			{
				size_t id = 0;
				item.insertInMap( id , new WorkerTaskItemGenerator( task_id , id ,  task ) );
			}
			
			return;
		}
		
		if( type == Operation::parser )
		{
			// An item per file
			assert( task.input_size() == 1);	// Only one input
			network::FileList fl = task.input(0);
			
			for (size_t i = 0 ; i < (size_t) fl.file_size() ; i++)
				item.insertInMap( i , new WorkerTaskItemParser( task_id , i , fl.file(i).name() , task ) );
			return;
		}
		
	}
	
	WorkerTaskItem *WorkerTask::getNextItemToProcess()
	{
		std::map<size_t,WorkerTaskItem*>::iterator iterator;
		for (iterator = item.begin() ; iterator != item.end() ; iterator++)
		{
			WorkerTaskItem *item = iterator->second;
			item->setup();
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