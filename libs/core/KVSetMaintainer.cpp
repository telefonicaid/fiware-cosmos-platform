#include "SSManager.h"
#include "KVSet.h"
#include "KVManager.h"
#include "MemoryController.h"
#include "SSLogger.h"
#include "TaskManager.h"
#include "SSMonitor.h"
#include "LockDebugger.h"        /* au::LockDebugger                         */
#include "KVSetMaintainer.h"     /* Own interface                            */



//#define DEBUG_KVSETMAINTAINER

namespace ss {

	pthread_t mantainer_thread;
	
	void* memory_thread_task(void* p)
	{
		
		au::LockDebugger::setThreadTitle(" KVMantainer " );
		
		// KVSetMaintiner tasks
		while( true )
		{
			KVSet *set = KVManager::shared()->getNextKVSetDiskOperation();
			
			assert( set );
			
			if( !set->isOnDisk() )
			{
				assert( set->isOnMemory() );
				set->flushToDisk();
				KVManager::shared()->notifyFinishFlushToDisk(set);
			}
			else
			{
				assert( !set->isOnMemory() );
				set->loadFromDisk();
				KVManager::shared()->notifyFinishLoadFromDisk(set);
			}
			
			TaskManager::shared()->wakeUpWorkers();
			
		}		
		return NULL;
	}
	

	void runKVSetMaintainer()
	{
		int ans = pthread_create(&mantainer_thread, NULL, memory_thread_task, NULL);	

		if (ans != 0)
		{
			LOG_ERROR(("pthread_create error"));
			assert(0);
		}	
	}
	
#pragma mark AUTOMATIC TASKS THREAD

	
	void* automatic_tasks(void*p)
	{
		
		au::LockDebugger::setThreadTitle(" KVQueues automatic process cheker " );
		
		while( !SSManager::shared()->finish )
		{
			// Review if new automatic tasks should be scheduled
			KVManager::shared()->scheduleAutomaticTasks();
			
			sleep(1);

		}
		return NULL;
	}
	
	pthread_t automatic_tasks_thread;
	void runAutomaticTasksScheduler()
	{
		int ans = pthread_create(&automatic_tasks_thread, NULL, automatic_tasks, NULL);	

		if (ans != 0)
		{
			LOG_ERROR(("pthread_create error"));
			assert(0);
		}	
	}
	

#pragma mark WORKERS Thread
	
#pragma mark CORE THREAD
	
	
	typedef struct
	{
		int core_id;
	} core_thread_task_info;
	
	/**
	 Task run by the core threads to operate over the task queu
	 */
	
	void* core_thread_task(void* p)
	{
		
		// Get the core id	
		core_thread_task_info *info = (core_thread_task_info*) p;
		int core_id = info->core_id;

		// Run the main tasks of the worker
		TaskManager::shared()->run_worker( core_id );

		return NULL;
	}
	
	
	
	pthread_t *workers_thread;
	
	void runTasksWorkers( int cores )
	{
		
		core_thread_task_info info;
		
		workers_thread = (pthread_t*) malloc( sizeof(pthread_t) * cores );
		
		for (int i =  0 ; i < cores ; i++)
		{
			info.core_id = i;
			int ans = pthread_create(&workers_thread[i], NULL, core_thread_task, &info);	

			if (ans != 0)
			{
				LOG_ERROR(("pthread_create error"));
				assert(0);
			}
		}
	}

	
#pragma mark MONITORIZATION THREAD
	
	pthread_t monitoring_thread;

	
	void* monitoring_thread_task(void* p)
	{
		SSMonitor * monitor = SSMonitor::shared();
		monitor->run();
		return NULL;
	}
	
	void runMonitorizationThread( )
	{
		//Monitoring thread
		int ans = pthread_create(&monitoring_thread, NULL, monitoring_thread_task, NULL);

		if (ans != 0)
		{
			LOG_ERROR(("pthread_create error"));
			assert(0);
		}	
	}
	
	
	
	
	
	
	
}

