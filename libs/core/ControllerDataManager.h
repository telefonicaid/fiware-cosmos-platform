#ifndef _H_CONTROLLER_DATA_MANAGER
#define _H_CONTROLLER_DATA_MANAGER

#include "Lock.h"				// au::Lock
#include <sstream>				// std::ostringstream
#include "samson/KVFormat.h"	// ss:: KVFormat
#include "Format.h"				// au::Format
#include <map>					// std::map
#include "DataManager.h"		// ss::DataManager

namespace ss {
	
	class ControllerQueue;
	
	/**
	 Data manager at the controller
	 */
	class ControllerTask;
	
	class ControllerDataManager : public DataManager
	{
		au::Lock lock;
		std::map< std::string , ControllerQueue*> queues;
		
	public:

		/**
		 Update internal status with this finished task
		 */
		
		void updateWithFinishedTask( ControllerTask *task );
		
		/**
		 Get a string describing status
		 Usefull for debuggin
		 */
		
		std::string status();
		
	private:
		
		bool _run( size_t task_id , std::string command );
		std::string getLogFileName( );
		
		
		
	};

}

#endif
