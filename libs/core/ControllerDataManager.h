#ifndef _H_CONTROLLER_DATA_MANAGER
#define _H_CONTROLLER_DATA_MANAGER

#include "Lock.h"				// au::Lock
#include <sstream>				// std::ostringstream
#include "samson/KVFormat.h"	// ss:: KVFormat
#include "Format.h"				// au::Format
#include <map>					// std::map
#include "DataManager.h"		// ss::DataManager
#include "au_map.h"				// au::map

namespace ss {
	
	class ControllerQueue;
	class ControllerTask;
	class DataManagerCommandResponse;
	class SamsonController;
	
	/**
	 Data manager at the controller
	 */
	
	class ControllerDataManager : public DataManager
	{
		au::Lock lock;
		au::map< std::string , ControllerQueue> queues;
		
		SamsonController *controller;	// Pointer to controller for module access
		
	public:
		
		ControllerDataManager( SamsonController *_controller ) : DataManager( getLogFileName()  )
		{
			controller = _controller;
		}

		/**
		 Update internal status with this finished task
		 */
		
		void updateWithFinishedTask( ControllerTask *task );
		
		/**
		 Get a string describing status
		 Usefull for debuggin
		 */
		
		std::string status();

		/**
		 Get the fileName of the log file
		 */
		static std::string getLogFileName( );

		
		/**
		 Check if a queue exist
		 */
		
		bool existQueue( std::string queue)
		{
			lock.lock();
			bool ans =  ( queues.findInMap( queue ) != NULL );
			lock.unlock();
			return ans;
		}
		
		std::string getQueuesStr( std::string format );
		
	private:
		
		virtual DataManagerCommandResponse _run( std::string command );
		virtual void _un_run( std::string command );
		
		
		
	};

}

#endif
