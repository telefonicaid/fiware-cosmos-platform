#include "logMsg.h"              // LM_*
#include "coreTracelevels.h"     // LMT_*

#include "iomMsgSend.h"          // iomMsgSend
#include "CommandLine.h"         // au::CommandLine
#include "SamsonWorker.h"        // SamsonWorker
#include "WorkerTaskManager.h"   // Own interface



namespace ss {

	/**
	 Return true if it is already finished
	 */
	
	bool WorkerTaskManager::addTask(const network::WorkerTask &worker_task )
	{
		// TODO: To be completed

		//size_t task_id = worker_task.task_id();
		//std::string operation = worker_task.operation();
		
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
