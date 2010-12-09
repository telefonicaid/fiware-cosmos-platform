

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
		
		
		// By default no error
		error = false;
		
		
		switch (type) {
			case Operation::generator :
				if( task.generator() )
					addItem( new WorkerTaskItemGenerator( task ) );
				break;
			case Operation::parser:
				{
					// An item per file
					assert( task.input_size() == 1);	// Only one input
					network::FileList fl = task.input(0);
					
					for (size_t i = 0 ; i < (size_t) fl.file_size() ; i++)
						addItem( new WorkerTaskItemParser( fl.file(i).name() , task ) );
				}
				break;
				
			case Operation::map :
			case Operation::parserOut :
			case Operation::reduce :
				addItem( new WorkerTaskItemOrganizer( task ) );
			default:
				break;
		}
		
		
		/**
		 Old code for indivual files ( to be removed )
		 
		 // An item per file
		 assert( task.input_size() == 1);	// Only one input
		 network::FileList fl = task.input(0);
		 
		 for (size_t i = 0 ; i < (size_t) fl.file_size() ; i++)
		 addItem( new WorkerTaskItemOperation( fl.file(i).name() , task ) );
*/		 
		
		
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
		
		i->setup();			// First setup ( usualy to get a slot of free memory and schedule load of inpute )
	}
	
	WorkerTaskItem *WorkerTask::getNextItemToProcess()
	{
		std::map<int,WorkerTaskItem*>::iterator iterator;
		for (iterator = item.begin() ; iterator != item.end() ; iterator++)
		{
			WorkerTaskItem *item = iterator->second;
			if( item->isReadyToRun() )
			{
				item->start();
				return item;
			}
		}
		return NULL;
	}
		
	void WorkerTask::setup()
	{		
		std::map<int,WorkerTaskItem*>::iterator iterator;
		for (iterator = item.begin() ; iterator != item.end() ; iterator++)
		{
			WorkerTaskItem *item = iterator->second;
			item->setup();
		}
	}
	
	bool WorkerTask::isFinish()
	{
		return ( item.size() == 0);	// No more items to process
	}
	
	void WorkerTask::getStatus( std::ostream &output , std::string prefix_per_line )
	{
		output << "ID:" << task_id << " Operation: " << operation;
		if( isFinish() )
			output << " [FINISH] ";
		output << std::endl;
		
		getStatusFromMap( output, item , prefix_per_line );
	}
	
	size_t WorkerTask::getId()
	{
		return task_id;
	}
	
	void WorkerTask::finishItem( size_t id  )
	{
		
		WorkerTaskItem *i = item.extractFromMap( id );
		
		if( i->error )
		{
			setError( i->error_message );
		}
		
		i->freeResources();
		delete i;
		
			
	}	
}