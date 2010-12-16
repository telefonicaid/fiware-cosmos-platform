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


namespace ss
{
	class LogFile;
	class DataManagerItem;
	
	class DataManager
	{
		LogFile file;							// File to write / read log data
		au::map<size_t,DataManagerItem> task;	// Map of current task with the list of previous commands

		size_t task_counter;					// Internal counter to give new task ids
		
	protected:
		
		au::Lock lock;	// Lock to protect multi-thread
		
	public:
		
		DataManager( std::string  fileName  ) : file( fileName ) 
		{
			task_counter = 0;
		}
		
		virtual ~DataManager() {}

		/**
		 Init function:
			* Recover previous logs from file updating "data"
			* Init a new session of the log ( clearign previous unfinished data )
		 */
		
		void init();

		// Static function to get "today" string
		static std::string today();
		
		size_t getNewTaskId()
		{
			lock.lock();
			size_t id = task_counter++;
			lock.unlock();
			return id;
		}
		
		/**
		 Function to interact with "data"
		 */
		
		void beginTask( size_t task_id , std::string command  );
		void finishTask( size_t task_id );
		void cancelTask( size_t task_id , std::string error );
		void addComment( size_t task_id , std::string comment);
		DataManagerCommandResponse runOperation( size_t task_id , std::string command );
		
	private:

		DataManagerItem* _beginTask( size_t task_id ,  std::string command ,  bool log );
		void _cancelTask( size_t task_id, std::string error , bool log );
		void _finishTask( size_t task_id, bool log );
		void _addComment( size_t task_id , std::string comment, bool log);
		DataManagerCommandResponse _runOperation( size_t task_id , std::string command , bool log);
		void _initSession( std::string command , bool log );
		void _clear();
		
		
	private:

		friend class DataManagerItem;
		
		/**
		 Unique interface to update the status of this DataManager
		 */
		
		virtual DataManagerCommandResponse _run( std::string command )=0;

		/**
		 Unique interface to undo a particular task
		 No error is possible here since we are just unding previous commands
		 */
		
		virtual void _un_run( std::string command )=0;
		
		
	};

}

#endif
