#include "ControllerTaskManager.h"		// Own interface
#include "SamsonController.h"			// ss::SamsonController
#include "CommandLine.h"				// au::CommandLine



namespace ss
{

	bool ControllerTaskManager::addTask( std::string command , std::ostringstream& output )
	{
		ControllerTask *_task=NULL;
		
		lock.lock();

		au::CommandLine commandLine;
		commandLine.parse(command);
		
		
		if( commandLine.get_num_arguments() > 0)
		{
			if( commandLine.get_argument(0) == "add_queue" )
			{
				if( checkAddQueueCommand( command , output ) ) 
				{
					_task = new ControllerTask( current_task_id++ ,  command , controller->workerEndPoints.size() );
					
					// Add individual command depending on the command
					// At the moment the same command is the unique line
					_task->addCommand( command );	
					
					task.insert( std::pair< size_t , ControllerTask*>( _task->getId()  , _task) );
					output << "Scheduled with global task id " << _task->getId();
				}
			}
		}
		
		lock.unlock();
	
		if( _task )
		{
			// Run task
			if( _task->isReady() )
			{
				_task->processCommand();
				controller->sendWorkerTasks( _task );	
			}
			
			return true;
		}
		else
			return false;
	}
	
	void ControllerTaskManager::notifyWorkerConfirmation( size_t task_id , int worker_id )
	{
		lock.lock();
		std::map< size_t , ControllerTask*>::iterator t =  task.find( task_id);
		if( t!= task.end() )
			t->second->notifyWorkerConfirmation( worker_id );
		lock.unlock();
		
	}
	
	
	
	bool ControllerTaskManager::checkAddQueueCommand(std::string command ,  std::ostringstream& output )
	{
		au::CommandLine cmdLine;
		cmdLine.parse(command);
		
		 // Add queue command
		 if( cmdLine.get_num_arguments() < 4 )
		 {
			 output << "Usage: add_queue name <keyFormat> <valueFormat>";
			 return false;
		 }
		 
		 std::string name = cmdLine.get_argument( 1 );
		 std::string keyFormat= cmdLine.get_argument( 2 );
		 std::string	valueFormat = cmdLine.get_argument( 3 );
		 
		 if( !controller->modulesManager.checkData( keyFormat ) )
		 {
			 output << "Unsupported data format " + keyFormat + "\n";
			 return false;
		 }
		 
		 if( !controller->modulesManager.checkData( valueFormat ) )
		 {
			 output << "Unsupported data format " + valueFormat + "\n";
			 return false;
		 }
		 
		return true;
	}
}
