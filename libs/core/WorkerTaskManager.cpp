
#include "WorkerTaskManager.h"			// Own interface
#include "SamsonWorker.h"				// ss:SamsonWorker

namespace ss {

	/**
	 Return true if it is already finished
	 */
	
	bool WorkerTaskManager::addTask(const network::WorkerTask &task )
	{
		
		std::string command = task.command();
		
		au::CommandLine commandLine;
		commandLine.parse(command);
		
		// Nothing if no main command
		if ( commandLine.get_num_arguments() == 0)
			return true;
		
		std::string mainCommand = commandLine.get_argument(0);
		
		// Different direct commands
		
		if( mainCommand == "add_queue" )
		{
			std::vector<std::string> queues;						// List of queues altered during this task
			queues.push_back( commandLine.get_argument(1) );		// updated queues in the process
			
			// Direct confirmation of the process
			worker->data.runOperationOfTask( task.task_id() , task.command() );
			return true;
		}
		
		// Unknown operation ( finished anyway )
		return true;
	}
	
	// Fill information about status of this worker
	void WorkerTaskManager::fillWorkerStatus( network::WorkerStatus* status )
	{
		network::WorkerSystem *system =  status->mutable_system();
		system->set_cores_total(10);
		system->set_cores_running(1);
	}
	

}