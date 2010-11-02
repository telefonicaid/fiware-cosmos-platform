#include "logMsg.h"                     // LM_*
#include "coreTracelevels.h"            // LMT_*

#include "iomMsgSend.h"                 // iomMsgSend
#include "SamsonWorker.h"				// ss:SamsonWorker
#include "WorkerTaskManager.h"			// Own interface



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
		else if (mainCommand == "task_test")
		{
			int       ix;
			Endpoint* cwP;
			Endpoint* me;

			LM_T(LMT_TASK, ("Sending task_test to all CoreWorkers"));

			me = worker->network->endpointLookup(0);
			if (me == NULL)
				LM_X(1, ("NULL me ..."));

			for (ix = 3; ix < worker->network->getNumEndpoints(); ix++)
			{				
				cwP = worker->network->endpointLookup(ix);

				if (cwP == NULL)
					continue;
				if (cwP->type != Endpoint::CoreWorker)
					continue;

				LM_T(LMT_TASK, ("Sending task_test to Core Worker '%s'", cwP->name.c_str()));
				iomMsgSend(cwP->fd, cwP->name.c_str(), me->name.c_str(), Message::WorkerTask, Message::Evt, (void*) mainCommand.c_str(), strlen(mainCommand.c_str()) + 1);
			}

			return true;
		}

		// Unknown operation ( finished anyway )
		return true;
	}
	
#if 0
	// Fill information about status of this worker
	void WorkerTaskManager::fillWorkerStatus( network::WorkerStatus* status )
	{
		network::WorkerSystem *system =  status->mutable_system();
		system->set_cores_total(10);
		system->set_cores_running(1);
	}
#endif
}
