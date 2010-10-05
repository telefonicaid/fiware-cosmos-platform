#ifndef SAMSON_KV_MANAGER_LOGS_H
#define SAMSON_KV_MANAGER_LOGS_H

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



/**
 Class to contain an activity log
 */

#define TASK_CODE_BEGIN				1
#define TASK_CODE_END				2
#define TASK_CODE_ACTION			3

namespace ss {

	
	class KVSetVector;
	class KVManager;
	
	class KVManagerLog : public protocol::KVManagerOperation
	{
	public:
		KVManagerLog( )
		{
		}
		
		KVManagerLog( size_t task_id , int task_code )
		{
			set_task( task_id );
			set_task_code( task_code );
		}
		
		// Set the parent ID
		
		void setParentTaskId( size_t id )
		{
			set_parenttask( id );
		}
		
		// Add queue
		void addQueue( std::string queue_name , KVFormat format );
		
		// Add sets
		void addKVSetVectorToKVQueue( KVSetVector* sets , std::string queue_name , int hash );	
		void addKVSetToKVQueue( KVSet* set , std::string queue_name , int hash );	
		
		// Remove queue
		void removeQueue( std::string queue_name  );
		
		// Remove sets
		void removeKVSetVectorToKVQueue( KVSetVector* sets , std::string queue_name , int hash );	
		
		// Add a queue and all its content ( prepare nice quit of the platform )
		void addQueue( KVQueue *_queue );
		
		
		// New KVSet
		void addKVSet( size_t set_id );
		
		// Notification that set has been saved to disk
		void addSavedToDisk( size_t set_id ); 
		
	};



	/** 
	 Class to hold the vector of logs for a particular top-level task
	 */

	class KVManagerLogBuffer
	{
		
	public:
		
		std::vector<KVManagerLog *> logs;
		
		size_t top_level_task;
		int counter_sets;			// Sets that need to be flushed to disk to validate this top-level task
		bool finish;				// The top-level end command has been received
		bool validated;				// the end has been received and all sets are down on disk
		
		
		KVManagerLogBuffer(size_t _top_level_task)
		{
			top_level_task = _top_level_task;
			counter_sets = 0;
			finish= false;
			validated = false;
		}
		
		void process( KVManagerLog *log );
		void runAll( KVManager *manager );
		
		
	};

	class KVManager;
	class KVManagerLogProcessor
	{
		KVManager *manager;
		
		
		// List of unexecuted logs waiting for a confirmation of a top-level log ( both finish and all sets saved )
		// They are mapped by top_level tasks  
		std::map< size_t , KVManagerLogBuffer *> logs;	
		
		
		std::map<size_t,size_t> parents;					// Map to keep the parent id of any task
		
	public:
		
		KVManagerLogProcessor( KVManager *_manager );
		
		void run( std::string fileName  );
		
		
		
	private:
		
		size_t getParentTask( size_t task_id );
		size_t getTopTask( size_t task_id );
		void setParentTask( size_t task_id , size_t parent_task_id);
		
		
		
	};



	#pragma mark Log stuff

	class KVManagerLogs
	{
		FILE *log;														//!< Log to save all activity for crash-recovery and hot-start
		
	protected:
		
		void setupFromLog( KVManager *manager );						//!< Restore previous state using log information
		virtual void runOperation( KVManagerLog *operation )=0;
		std::string _logFileName( );
		void _logOperation( KVManagerLog *operation , FILE *l );
		void _logOperation( KVManagerLog *operation );
		void _restoreWithUniqueLog( KVManagerLog *log );
		
		void _startLog();
		void _stopLog();
		
	};

}

#endif
