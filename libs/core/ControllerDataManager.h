#ifndef _H_CONTROLLER_DATA_MANAGER
#define _H_CONTROLLER_DATA_MANAGER

#include "Lock.h"				// au::Lock
#include <sstream>				// std::ostringstream
#include "samson/KVFormat.h"	// ss:: KVFormat
#include "Format.h"				// au::Format
#include <map>					// std::map
#include "DataManager.h"		// ss::DataManager

namespace ss {
	
	/**
	 Information contained in the controller about a queue
	 */
	
	class ControllerQueue
	{
		std::string _name;				// Name of this queue
		KVFormat _format;				// Format of the queue
		
	public:
		ControllerQueue( std::string name , KVFormat format )
		{
			_name = name;
			_format = format;
		}
		
		std::string str()
		{
			std::ostringstream o;
			o << "(" << _format.str() << ") ";
			return o.str();		
		}
		
	};
	
	/**
	 Data manager at the controller
	 */
	class ControllerTask;
	
	class ControllerDataManager : public DataManager
	{
		au::Lock lock;
		std::map< std::string , ControllerQueue*> queue;	// Queues in the system
		
		
	public:

		/**
		 Update internal status with this finished task
		 */
		
		void updateWithFinishedTask( ControllerTask *task );
		
		/**
		 Get a string describing status
		 Usefull for debuggin
		 */
		
		std::string status()
		{
			std::ostringstream o;
			o << "Data Manager:" << std::endl;
			lock.lock();
			for ( std::map< std::string , ControllerQueue*>::iterator q = queue.begin() ; q != queue.end() ; q++)
				o << q->first << " " << q->second->str() << std::endl;
			lock.unlock();
			
			return o.str();
			
		}		
		
	private:
		
		bool _run( size_t task_id , std::string command );
		std::string getLogFileName( );
		
		
		
	};

}

#endif
