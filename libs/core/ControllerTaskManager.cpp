#include "ControllerTaskManager.h"		// Own interface
#include "ControllerTask.h"				// ss::ControllerTask
#include "SamsonController.h"			// ss::SamsonController
#include "CommandLine.h"				// au::CommandLine
#include "Endpoint.h"					// ss::Endpoint
#include "Packet.h"						// ss::Packet

namespace ss
{

	ControllerTask* ControllerTaskManager::createTask(int fromIdentifier, std::string command , std::ostringstream& output )
	{
		ControllerTask *_task = NULL;
		
		au::CommandLine commandLine;
		commandLine.parse(command);
		
		// Check everything is ok
		
		if( commandLine.get_num_arguments() > 0)
		{
			if( commandLine.get_argument(0) == "add_queue" )
			{
				
				// Add queue command
				if( commandLine.get_num_arguments() < 4 )
				{
					output << "Usage: add_queue name <keyFormat> <valueFormat>";
					return false;
				}
				
				std::string name = commandLine.get_argument( 1 );
				std::string keyFormat= commandLine.get_argument( 2 );
				std::string	valueFormat = commandLine.get_argument( 3 );
				
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
				
				// Create a top level task
				_task = new ControllerTask( fromIdentifier, 0, current_task_id++ ,  command , controller->network->getNumWorkers() );
				
			}
			else if (commandLine.get_argument(0) == "task_test")
			{
				// Create a top level task
				_task = new ControllerTask( fromIdentifier, 0, current_task_id++ ,  command , controller->network->getNumWorkers() );
			}
		}

		
		return _task;
	}
	
	bool ControllerTaskManager::addTask( int fromIdentifier, std::string command , std::ostringstream& output )
	{
		lock.lock();
		bool answer = _addTask( fromIdentifier , command , output );
		lock.unlock();
		return answer;
	}

	
	bool ControllerTaskManager::_addTask( int fromIdentifier, std::string command , std::ostringstream& output )
	{
		ControllerTask *_task = createTask( fromIdentifier, command , output );
		
		
		if( _task )
		{			
			// Add the task to the list
			task.insert( std::pair< size_t , ControllerTask*>( _task->getId() , _task) );
			
			
			output << "Scheduled with global task id " << _task->getId();
			
			// Put the task in running by sending to all workers
			controller->sendWorkerTasks( _task );								// Send the command to all the workers to perform this task
			_task->setRunning();												// Put the task to running
			controller->data.runOperationOfTask( _task->getId(), _task->getCommand() );	// We process internally in the local "data manager"
			
			return true;
		}
		else
		{
			output << "Unknown operation " << command;
			return false;
		}
	}
	
	void ControllerTaskManager::notifyWorkerConfirmation( int worker_id, network::WorkerTaskConfirmation confirmationMessage )
	{
		// get the task id that is confirmed
		size_t task_id = confirmationMessage.task_id();
		
		lock.lock();
		std::map< size_t , ControllerTask*>::iterator t =  task.find( task_id);
		if( t!= task.end() )
		{
			ControllerTask *_task = t->second;
			
			_task->notifyWorkerConfirmation( worker_id , confirmationMessage );
			
			
			if ( _task->isFinished() )
			{
				if( _task->isTopLevelTask() )
				{
					// Send a message back to the dalilah that ordered this comman ( if still connected )
					controller->sendDelilahAnswer( _task->getId() , _task->getFromIdentifier(), false, true,  "OK!");
				}

				// Update this in the data controller
				controller->data.commit( _task->getId() );
				
				// Delete the task from the task manager
				delete t->second;
				task.erase( t );
			}
			
		}
		lock.unlock();
		
	}
	
	
	
	std::string ControllerTaskManager::status()
	{
		std::stringstream o;
		o << "Task Manager:" << std::endl;
		std::map< size_t , ControllerTask*>::iterator t;
		for (t = task.begin() ; t != task.end() ; t++)
			o << t->second->str() << std::endl;
		return o.str();
		
	}
	
	
}
