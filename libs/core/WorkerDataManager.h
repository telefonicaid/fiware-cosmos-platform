#ifndef _H_WORKER_DATA_MANAGER
#define _H_WORKER_DATA_MANAGER

#include <map>					// std::map
#include "Lock.h"				// au::Lock
#include "samson/KVFormat.h"	// ss::KVFormat
#include "KVInfo.h"				// ss:KVInfo
#include <sstream>				// std::ostringstream
#include "samson.pb.h"			// network::Update
#include <vector>				// std::vector
#include "DataManager.h"		// ss::DataManager

namespace ss {
	

	class WorkerQueue;
	
	/**
	 WorkerDataManager main class to sore data manager of a SAMSON_WORKER
	 */
	class SamsonWorker;

	
	class WorkerDataManager : public DataManager
	{
		// Lis of queues managed by this worker
		std::map< std::string , WorkerQueue*> queues;
		
		// Pointer to the worker to use other components
		SamsonWorker *worker;
		
	public:
		
		WorkerDataManager( SamsonWorker *_worker )
		{
			// Keep a pointer to the worker
			worker = _worker;
		}

		
		
		std::string getLogFileName( );
		
		void fillWorkerStatus( network::WorkerStatus* status );
		
	private:
		
		/**
		 Real implementation of process protected by "lock"
		 */
		virtual bool _run( size_t task_id , std::string command );

		
		
		
	};

}


#endif