#ifndef _H_DATA_MANAGER
#define _H_DATA_MANAGER

#include <iostream>
#include <fstream>							// ifstream , ofstream

#include "logMsg.h"							// LM_*
#include "dataTracelevels.h"				// LMT_*

#include "data.pb.h"
#include "traces.h"
#include "Lock.h"							// au::Lock
#include "au_map.h"							// au::map
#include <sstream>							// std::ostringstream
#include "LogFile.h"						// ss::LogFile
#include "DataManagerCommandResponse.h"		// ss::DataManagerCommandResponse
#include <set>								// std::set

namespace ss
{
	class LogFile;
	class DataManagerItem;
	
	class DataManager
	{
		LogFile file;							// File to write
		size_t task_counter;					// Internal counter to give new task ids
		
		std::set<size_t> active_tasks;			// Set of active tasks ( to ingnore non active tasks commands )
		
	protected:
		
		au::Lock lock;							// Lock to protect multi-thread
		
	public:
		
		DataManager( std::string  fileName );
		
		virtual ~DataManager() {}

		// Init session
		void initSession();
		
		/**
		 Function to interact with "data"
		 */
		
		void beginTask( size_t task_id , std::string command  );
		void finishTask( size_t task_id );
		void cancelTask( size_t task_id , std::string error );
		void addComment( size_t task_id , std::string comment);
		
		DataManagerCommandResponse runOperation( size_t task_id , std::string command );
		
	protected:
		DataManagerCommandResponse _runOperation( size_t task_id , std::string command );

		
	protected:
		
		size_t _getNewTaskId()
		{
			return task_counter++;
		}
		
	public:
		
		size_t getNewTaskId()
		{
			lock.lock();
			size_t id = _getNewTaskId();
			lock.unlock();
			return id;
		}
		
	private:

		friend class DataManagerItem;
		
		/**
		 Unique interface to update the status of this DataManager
		 */

		virtual void _clear()=0;
		virtual DataManagerCommandResponse _run( std::string command )=0;
		
		// Static function to get "today" string
		static std::string today();
		

		// Reload data form file
		void _reloadData();
		
	};

}

#endif
