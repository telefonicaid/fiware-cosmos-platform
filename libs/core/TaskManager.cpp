#include "TaskManager.h"
#include "KVManager.h"
#include "SSManager.h"
#include "SSLogger.h"
#include "SSMonitor.h"
#include "NetworkManager.h"



namespace ss {

	static TaskManager *taskManager = NULL;

	TaskManager * TaskManager::shared()
	{
		if( !taskManager )
			taskManager = new TaskManager();
		return taskManager;
	}
	
	
#pragma mark Interface with the workers
	
	Task* TaskManager::getNextTask( int core_id )
	{
		lock.lock();
		
		for ( std::list<Task*>::iterator i = _tasks.begin() ; i != _tasks.end() ; i++ )
		{
			Task *task = *i;
			if( _start_process( task ) )
			{
				// Mark the task as running to not be taken by other process
				task->status = ss::running;	
				
				// Task is not removed from the queue until it is completelly finished
				num_working_cores++;
				lock.unlock();
				return task;
			}
		}
		
		lock.unlock();

		return NULL;
		
	}	
	
	void TaskManager::notifyTaskFinish( Task *task  , bool finish )
	{
		lock.lock();
		_stop_process( task , finish );
		lock.unlock();
	}
	
	
	

#pragma mark Worker tasks
	
	void TaskManager::run_worker( int core_id )
	{
		std::ostringstream o;
		o << "Task Process " << core_id;
		au::LockDebugger::setThreadTitle( o.str() );
		
		while( true )	
		{
			// Get next task
			
			Task* task=NULL;
			do
			{
				task = taskManager->getNextTask( core_id );	
				if( !task)
				{
					lock.lock();
					lock.unlock_waiting_in_stopLock( &stopLock_cores  );
				}
			} while( !task );
			
			
			SSLogger::log( task , SSLogger::message ,  "Begin task" );
			
			// Run the task itself
			bool task_finished = task->run();
			
			SSLogger::log( task , SSLogger::message , "Stop task" );
			
			// Notify that this tasks is stop
			notifyTaskFinish( task , task_finished );
				
		}		
	}
	
#pragma mark RUN COMMAND
	
	CommandResponse TaskManager::runCommand( Task *parentTask , std::string command , bool preferent_task )
	{
		CommandResponse response;
		response.error = false;
		
		// Identify if this is a synchronous command and run it
		std::stringstream output;
		size_t task_id = parentTask?parentTask->getId():0;
		
		if( _runSynchCommand( task_id , command , output , response.error ) )
		{
			response.message = output.str();
			return response;
		}
		
		// Create the task for this command
		lock.lock();
		
		Task *task = new Task( command , task_counter++, parentTask );

		if( task->isError() )
		{
			lock.unlock();

			response.error = true;
			response.message = task->getErrorMessage();
			delete task;
			return response;
		}
		
		// Notify that the task is scheduled
		KVManager::shared()->notifyNewTask( task );			// Notify the KVManager a new tasks is scheduled

		// Prepare KVSets asks the KVManager for necessary KVSets ( removing if necessary )
		task->prepareKVSets();
		
		// Add the task to the scheduler
		_addTask(  task , preferent_task );
		
		output << "Task with id " << task->getId() << " squeduled\n";
		response.message = output.str();
		
		lock.unlock();
		
		wakeUpWorkers();
		
		return response;
		
	}
		
	
	bool TaskManager::_runSynchCommand( size_t taskId ,  std::string command  , std::stringstream &output , bool& error )
	{

		AUCommandLine _cmdLine;				//!< Parser used to analyse the command sentence
		
		// Common parse options
		_cmdLine.set_flag_boolean("remove");		// Flag to indicate that original sets should be removed from the input queue
		_cmdLine.set_flag_boolean("sub_operation");	// Flag to indicate that it is a suboperation ( for every hash )
		_cmdLine.set_flag_int("hash", -1);			// In suboperations, it is used to indicate the hash to process
		_cmdLine.set_flag_int("n", 1);
		_cmdLine.set_flag_boolean("no-state");
		_cmdLine.set_flag_int("scale", 0);
		
		// Parse the command line
		_cmdLine.parse( command );
		
		
		if( _cmdLine.get_num_arguments() == 0)
			return true;
		
		if( ( _cmdLine.get_argument(0) == "quit" ) || ( _cmdLine.get_argument(0) == "quit_system" ) )
		{
			TaskManager::shared()->quit();	// Prepare everything for a "good" goodbye ( no pending tasks )
			KVManager::shared()->quit();	// Prepare everything for a "good" goodbye ( no sets only on memory )
			output << "Finish command OK\n";
			return true;
		}
		
		if( ( _cmdLine.get_argument(0) == "memory" ) || ( _cmdLine.get_argument(0) == "m" ) )
		{
			output << KVManager::shared()->strMemory();
			return true;
		}
		if( ( _cmdLine.get_argument(0) == "sets" ) || ( _cmdLine.get_argument(0) == "s" ) )
		{
			output << KVManager::shared()->showSets();
			return true;
		}
		
		if( ( _cmdLine.get_argument(0) == "queues" ) || ( _cmdLine.get_argument(0) == "q" )  )
		{
			output << KVManager::shared()->showQueues();
			return true;
		}
		
		if( ( _cmdLine.get_argument(0) == "process" ) || ( _cmdLine.get_argument(0) == "p" )  )
		{
			output << KVManager::shared()->showProcess();
			return true;
		}
		
		if( ( _cmdLine.get_argument(0) == "tasks" ) || ( _cmdLine.get_argument(0) == "t" )  )
		{
			output << TaskManager::shared()->showTasks();
			return true;
		}
		if( ( _cmdLine.get_argument(0) == "connections" ) || ( _cmdLine.get_argument(0) == "c" )  )
		{
			output << NetworkManager::shared()->showConnections();
			return true;
		}
		
		if( ( _cmdLine.get_argument(0) == "purge_connections" ) || ( _cmdLine.get_argument(0) == "pc" )  )
		{
			NetworkManager::shared()->purgeConnections();
			output << "OK\n";
			return true;
		}
		
		if( ( _cmdLine.get_argument(0) == "reload" ) )
		{
			ModulesManager::shared()->reloadModules();
			output << "OK\n";
			return true;
		}
		
		if( ( _cmdLine.get_argument(0) == "get" ) )
		{
			output << SSMonitor::shared()->get();
			return true;
		}
		
		if( ( _cmdLine.get_argument(0) == "add_queue" ) )
		{
			if( _cmdLine.get_num_arguments() < 4 )
			{
				return "Usage: add_queue name <keyFormat> <valueFormat>\n";
			}
			
			std::string name = _cmdLine.get_argument( 1 );
			std::string keyFormat= _cmdLine.get_argument( 2 );
			std::string	valueFormat = _cmdLine.get_argument( 3 );
			
			if( !ModulesManager::shared()->checkData( keyFormat ) )
			{
				output << "Unsupported data format " + keyFormat + "\n";
				return true;
			}
			
			if( !ModulesManager::shared()->checkData( valueFormat ) )
			{
				output << "Unsupported data format " + valueFormat + "\n";
				error = true;
				return true;
			}
			
			KVManager::shared()->newKVQueue(taskId , name, KVFormat::format( keyFormat , valueFormat ) );
			output << "OK\n";
			return true;
		}
		
		if( ( _cmdLine.get_argument(0) == "remove_queue" ) )
		{
			if( _cmdLine.get_num_arguments() < 2 )
			{
				output << "Usage: remove_queue name \n";
				error = true;
				return true;
			}
			
			std::string name = _cmdLine.get_argument( 1 );
			
			KVManager::shared()->removeKVQueue(taskId , name );
			output << "OK\n";
			return true;
		}		
		
		if( ( _cmdLine.get_argument(0) == "add_connection" ) )
		{
			if( _cmdLine.get_num_arguments() < 3 )
			{
				output << "Usage: add_connection queue queue1 queue2 ...\n";
				error = true;
				return true;
			}
			std::vector<KVFormat> formats;
			
			std::string queue_name = _cmdLine.get_argument( 1 );
			formats.push_back( KVManager::shared()->getKVQueueFormat( queue_name ) );
			std::vector<std::string> connected_queue_names;
			for (int  i=2 ; i < _cmdLine.get_num_arguments() ; i++)
			{
				std::string _queue_name = _cmdLine.get_argument(i);
				connected_queue_names.push_back( _queue_name  );
				formats.push_back(  KVManager::shared()->getKVQueueFormat( _queue_name ) );
			}

			// Make sure all are the same format
			for (size_t i = 0 ; i < (formats.size()-1) ; i++)
				if( !formats[i].isEqual( formats[i+1] ) )
				{
					output << "It is not possible to connect queues with different formats";
					error = true;
					return true;	// True means that this was a sync command
				}
													
			
			KVManager::shared()->connectQueues(queue_name, connected_queue_names  );
			output << "OK\n";			
			return true;
		}
		
		
		if( ( _cmdLine.get_argument(0) == "add_process" ) )
		{
			if( _cmdLine.get_num_arguments() < 3 )
			{
				output << "Usage: add_process queue_name operation outputs" << std::endl;
				error = true;
				return true;
			}
			
			std::string queue_name = _cmdLine.get_argument( 1 );
			std::string operation_name = _cmdLine.get_argument( 2 );
			
			std::vector <std::string> output_queues;
			for (int i = 3 ; i < _cmdLine.get_num_arguments() ; i++)
				output_queues.push_back( _cmdLine.get_argument(i) );
			
			KVManager::shared()->addProcess(  queue_name, operation_name , output_queues );
			return "OK\n";
		}
		
		
		if( ( _cmdLine.get_argument(0) == "help" )   )
		{
			
			std::ostringstream o;
			
			
			if( _cmdLine.get_num_arguments() > 1)
			{
				
				std::string mainHelpCommand = _cmdLine.get_argument(1);
				
				if( mainHelpCommand	== "modules" )
				{
					output << ModulesManager::shared()->showModules();
					return true;
				}
				
				
				if( mainHelpCommand == "find" )
				{
					output <<  ModulesManager::shared()->showFind( command );
					error = true;
					return true;
				}
				
				
				// Help about an operation
				Operation *operation = ModulesManager::shared()->getOperation( mainHelpCommand );
				if( operation )
				{
					output << operation->strHelp();
					return true;
				}
				
				// Help about a module
				if(  ModulesManager::shared()->getModule( mainHelpCommand ) )
				{
					output <<  ModulesManager::shared()->showModule( mainHelpCommand );
					return true;
				}
				
				
				output << "No help for " << _cmdLine.get_argument(1) << std::endl;
				error = true;
				return true;
			}
			
			// General help message without any parameters
			
			output << "--------------------------------------------------------------------------------" << std::endl;
			output << "SAMSON v." << SAMSON_VERSION << std::endl;
			output << "--------------------------------------------------------------------------------" << std::endl;
			output << "Samson is a C++ based map&reduce platform designed for Telefónica I+D"			<< std::endl;
			output << "Auth: Andreu Urruela (andreu@tid.es)" << std::endl;
			output << "--------------------------------------------------------------------------------" << std::endl;
			output << std::endl;
			output << "add_queue          Add a queue to the platform" << std::endl;
			output << "                   Usage: add_queue <queue_name> <keyFormat> <valueFormat>" << std::endl;
			output << std::endl;
			output << "help modules       Show installed modules with data-types and operations" << std::endl;
			output << "queues             Show current queues" << std::endl;
			output << "tasks              Show current tasks ( scheduled or running )" << std::endl;
			output << std::endl;
			output << "help               Show this help message "<< std::endl;
			output << std::endl;
			output << "--------------------------------------------------------------------------------" << std::endl;
			output << "Current log file: " << SSLogger::shared()->logFile << std::endl;
			output << "--------------------------------------------------------------------------------" << std::endl;
			
			return true;
		}
		
		// It is not a synch tasks and should be schedulled if format is correct
		return false;
	}
	
#pragma mark Add tasks
			
	size_t TaskManager::_addTask( Task * task , bool preferent_task )
	{
		assert( task->status == ss::definition );		// Only taks in definition state are accepted	

		Task *parentTask = task->getParentTask();
		if( parentTask )
			parentTask->numberChildren++;

		// Set the new status
		task->status = ss::in_queue;					// Now it will be in the queue

		// Add to the queue
		if( preferent_task )
			_tasks.push_front( task );
		else
			_tasks.push_back( task );
			
		return task->getId();
		
	}
			
	
	bool TaskManager::_start_process( Task * task )
	{
		
		// Return if the task is ready and free of dependencies
#ifdef DEBUG_TASK_MANAGER
		std::cout << "TASK_MANAGER: Considering if ready a task " << this << std::endl;
#endif
		
		if( ( task->status == ss::running ) || ( task->status == ss::finished ) || (task->status == ss::waiting) )
		{
#ifdef DEBUG_TASK_MANAGER
			std::cout << "TASK_MANAGER: Discarted since it is running / finished / waiting " << this << std::endl;
#endif
			return false;
		}
		
		// Task is suppoused to be in the queue or waiting
		assert( task->status == ss::in_queue );	
		
		if( task->numberChildren > 0)	// this discart taks waiting for children tasks
		{
#ifdef DEBUG_TASK_MANAGER
			std::cout << "TASK_MANAGER: Discarted since there are still pending children " << this << std::endl;
#endif
			return false;
		}
		
		// Let's check if we can block all KVSets / KVStorage
		if( ! KVManager::shared()->retainResources( task ) )
			return false;
		
		return true;			
	}
	
	void TaskManager::_stop_process( Task * task ,  bool finished )	
	{
		
		// Whatever is necessary to do when a task if finished
		assert( task->status == ss::running );	// The task is suppoused to be finished
		
		// We will not require this KVSets in memory anymore
		KVManager::shared()->releaseResources( task );
		
		if( finished )
		{
			
			if( task->numberChildren > 0 )
			{
				task->status = ss::waiting;
				SSLogger::log( task , SSLogger::message ,  "Finish but put in waiting since there are pending children" );
			}
			else
			{
				task->status = ss::finished;
				SSLogger::log( task , SSLogger::message ,  "Finish and removed from queue" );
				
				_removeTask( task);
			}
			
			
		}
		else
			task->status = ss::in_queue;	// Come back to the queue to run again when children are finished
				
		
	}
	
	void TaskManager::_removeTask( Task* task )
	{
		
		// Notify the KVMnager to free KVSets
		KVManager::shared()->notifyFinishedTask( task );
		
		// Let's notify the parent task
		Task *parentTask = task->getParentTask();
		if ( parentTask )
		{
			parentTask->numberChildren--;
			
			if ( ( parentTask->status == waiting) && ( parentTask->numberChildren == 0) )
				_removeTask(parentTask);
			
		}
		
		_tasks.remove( task );
		delete task ;			// Responsible to remove the element itself
	}
	

#pragma mark Show tasks for debugging
	
	std::string TaskManager::showTasks()
	{
		std::ostringstream o;
		
		lock.lock();
		
		int num_tasks_running = 0;
		int num_tasks_queue = 0;
		int num_tasks_waiting = 0;
		
		for (std::list<Task*>::iterator i = _tasks.begin() ; i != _tasks.end() ; i++)
		{
			Task *task = *i;
			switch (task->status) {
				case running: num_tasks_running++; break;
				case waiting: num_tasks_waiting++; break;
				case in_queue: num_tasks_queue++; break;
				default: break;
			}
			o << task->str() << std::endl;
		}
		
		
		lock.unlock();
		
		return o.str();
	}	
	
	
}

