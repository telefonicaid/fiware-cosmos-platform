#include "ControllerTaskManager.h"		// Own interface
#include "ControllerTask.h"				// ss::ControllerTask
#include "SamsonController.h"			// ss::SamsonController
#include "CommandLine.h"				// au::CommandLine
#include "Endpoint.h"					// ss::Endpoint
#include "Packet.h"						// ss::Packet

namespace ss
{

	bool ControllerTaskManager::addTask( int fromIdentifier, std::string command , std::ostringstream& output )
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
					// Create the tast adding the necessary commands inside
					_task = new ControllerTask( fromIdentifier,  current_task_id++ ,  command , controller->network->getNumWorkers() );
					_task->addCommand( command );	

					// Insert the task into the list
					task.insert( std::pair< size_t , ControllerTask*>( _task->getId()  , _task) );
					output << "Scheduled with global task id " << _task->getId();
				}
			}
		}
		
		lock.unlock();
	
		if( _task )
		{
			// Run task
			_task->run();
			
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
		{
			t->second->notifyWorkerConfirmation( worker_id );
			
			if (t->second->isFinished())
			{
				// Send a message back to the dalilah that ordered this comman ( if still connected )
				controller->sendDalilahAnswer( t->second->getId() , t->second->getFromIdentifier(), false, true,  "OK!");

				// Update this in the data controller
				controller->data.updateWithFinishedTask( t->second );
				
				// Delete the task from the task manager
				delete t->second;
				task.erase( t );
			}
			
		}
		lock.unlock();
		
	}
	
	bool ControllerTaskManager::checkAddQueueCommand(std::string command ,  std::ostringstream& output )
	{
		
		std::cout << "COMMAND :" << command;
		
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
