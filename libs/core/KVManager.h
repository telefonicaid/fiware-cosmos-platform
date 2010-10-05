#ifndef SAMSON_KV_MANAGER_H
#define SAMSON_KV_MANAGER_H

/**
 SAMSON key-value Manager
 */

#include <map>
#include <queue>
#include <set>
#include <list>
#include <iostream>
#include <assert.h>
#include <pthread.h>
#include <cstdlib>
#include <fstream>
#include <pthread.h>
#include <sys/stat.h>

#include "KVSetsManager.h"
#include "KVQueuesManager.h"
#include "MemoryController.h"
#include "KVManagerLogs.h"
#include <samson.pb.h>
#include "AULocker.h"
#include "Format.h"



//#define DEBUG_KV_MANAGER

namespace ss {

	class KVManager;
	class KVStorage;
	class KVSet;
	class KVQueue;
	class KVFormat;
	class TaskBase;
	class MonitoringInfo;
	class MemoryController;
	class KVQueuesManager;
	class KVSetBuffer;
	class KVQueueProcess;
	class KVSetVector;
	class KVQueueWriter;
	class Task;
	class KVSetBufferBase;
	
	/**
	 Manager of KVSets and KVQueues
	 */
		
	class KVManager :  public KVManagerLogs
	{
		au::Lock lock;								//!< Common lock to work with KVSets / KVQueues
		au::StopLock kvMantainerStopLock;			//!< StopLock for KVMantainer

		KVSetsManager sets;								//!< List of KVSets
		KVQueuesManager queues;						//!< Manager of the queues
		
		std::vector<KVQueueProcess*> process;		//!< List of process for automatic operations
		MemoryController memoryController;			//!< Memory controller to monitor used memory
		
	private:
		KVManager();								//!< Private constructor for singleton implementation

	public:	

		/**
		 Unique access to this singleton
		 */
		static KVManager* shared();	
		
		/**
		 Run operation described in the log file
		 */
		void runOperation( KVManagerLog *operation );
		
		/**
		 Add a KVSet to the manager.
		 This buffer can be flushed to disk and event free from memory if not retained by other tasks
		 */		 
		KVSet* addKVSet( size_t task_id , KVSetBufferBase *buffer );			

		/**
		 Add a particular KVSet to a queue
		 */		
		void addKVSetToKVQueue( size_t task_id , KVSet *set , std::string queue_name , int hash );			
		
		/**
			Notify a particular task is scheduled ( affect the order of the KVSets to be free from memory or load from disk )
		 */
		void notifyNewTask( Task* task );
		
		/**
		 Notify that a particular task has been finished ( and all the children process )
		 */
		void notifyFinishedTask( Task* task );

		/**
		 Get a vector containing all the KVSets of a partiuclar queue-hash
		 */
		KVSetVector * getKVSetsForTask( Task *task, std::string queue_name , int hash , bool remove_originals );
		
		/**
		 Get a single KVSet from a particular queue-hash for debugging
		 */
		KVSetVector * getAnyKVSetForTask( Task *task , std::string queue_name );		
		
		/**
		 Get a writing structure to a parciular queue
		 */		
		KVQueueWriter* getKVQueueWriter( Task* task ,  std::string queue_name );
		
		/**
		 Retain sets in memory the necessary KVSets for this tasks 
		 Return true if possible ( all KVSets are in memory )
		 */
		
		bool retainResources( Task* task );		// If possible, it return true and retain elements
		
		/**
		 Release sets from the memory-block
		 From now on, all KVSets can be free from memory if required ( and no other tasks using )
		 */
		
		void releaseResources( Task* task );	// Release KVSets
		

#pragma mark DISK OPERATION
		
		/**
		 System operation for memory management
		 Next KVSet to be flush to disk or load from disk
		 */
		
		KVSet * getNextKVSetDiskOperation();
		void notifyFinishFlushToDisk( KVSet *set );
		void notifyFinishLoadFromDisk( KVSet *set );
		
#pragma mark QUEUES

		void newKVQueue( size_t task_id,  std::string name , KVFormat format );
		KVFormat getKVQueueFormat( std::string name );
		void removeKVQueue( size_t task_id, std::string name );
		void connectQueues( std::string queue_name, std::vector<std::string> connected_queue_names  );
		std::string showQueues();

				
#pragma mark PROCESS
		
	public:
		void addProcess( std::string queue_name , std::string operation_name , std::vector<std::string> output_queues );
		void scheduleAutomaticTasks();
		std::string showProcess();
		
#pragma mark SETS
		
		std::string showSets();

#pragma mark Memory management
		
		/**
		 Add available memory to the system
		 Usually during bootstrap
		 */
		
		void addAvailableMemory( size_t memory );
		
		/** 
		 Alloc and Dealloc memory: 
			It is a blocking call if there is no memory
			Some KVSets may be free from memory to hold new data
		 */
		
		void alloc( size_t size  );		
		void dealloc( size_t size  );		
		
		std::string strMemory();
		
#pragma mark quit function
				
		void quit()
		{
			
			// Wait until all KVSets are on disk
			bool all_on_disk = false;
			while( !all_on_disk )
			{
				std::cout << "Waiting to be all on disk\n";
				
				lock.lock();
				all_on_disk = sets.all_on_disk();
				lock.unlock();
			
				sleep(1);
			}
			std::cout << "Everything on disk\n";
			
			// Restore log file for fast-startup
			lock.lock();
			_stopLog();
			
			// A single log with all information on the platform
			KVManagerLog * tmp = queues.getFullLog();
			_restoreWithUniqueLog( tmp );
			delete tmp;
			
			lock.unlock();
			
		}
	
		
	private:
		void _restoreOperationLog();							
	};
}

#endif
