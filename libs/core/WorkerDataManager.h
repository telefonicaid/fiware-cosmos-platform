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
	
	/**
	 Information contained in the controller about a queue
	 */
	
	class WorkerQueue
	{
		std::string _name;				// Name of this queue
		KVFormat _format;
		
		KVInfo info;					// Information about this queue 
		
		std::vector<std::string> fileName;
		
	public:
		WorkerQueue( std::string name , KVFormat format )
		{
			_name = name;
			_format = format;
		}
		
		std::string str()
		{
			std::ostringstream o;
			o << "(" << _format.str() << ") ";
			o << info.str();
			return o.str();		
		}
		
		void addFile(std::string _fileName )
		{
			fileName.push_back(_fileName);
		}
	
		void clear()
		{
			fileName.clear();
		}
		
		void rename( std::string name )
		{
			_name = name;
		}
		
		
		KVInfo getInfo()
		{
			return info;
		}
	};	
	
	
	
	/**
	 WorkerDataManager main class to sore data manager of a SAMSON_WORKER
	 */
	class SamsonWorker;

	
	class WorkerDataManager : public DataManager
	{
		// Lis of queues managed by this worker
		std::map< std::string , WorkerQueue*> queue;
		
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

		
	private:
		
		/**
		 Find a queue
		 */
		WorkerQueue *findQueue( std::string name )
		{
			std::map< std::string , WorkerQueue*>::iterator q = queue.find( name );
			if( q == queue.end() )
				return NULL;
			else
				return q->second;

		}
		
		
	};

}


#endif