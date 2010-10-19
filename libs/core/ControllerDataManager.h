#ifndef _H_CONTROLLER_DATA_MANAGER
#define _H_CONTROLLER_DATA_MANAGER

#include "Lock.h"				// au::Lock
#include <sstream>				// std::ostringstream
#include "samson/KVFormat.h"	// ss:: KVFormat
#include "Format.h"				// au::Format
#include <map>					// std::map

namespace ss {
	
	class KVInfo  
	{
	public:
		size_t size;
		size_t kvs;
		
		KVInfo()
		{
			size=0;
			kvs=0;
		}
		
		KVInfo( std::vector<KVInfo>& info)
		{
			size=0;
			kvs=0;
			for (size_t i = 0 ; i < info.size() ; i++)
			{
				size += info[i].size;
				kvs += info[i].kvs;
			}
		}
		
		std::string str()
		{
			std::ostringstream o;
			o <<au::Format::string( kvs ) << "kvs in " <<  au::Format::string( size ) << " bytes ";
			return o.str();
		}
	
		
		
	};
	
	
	/**
	 Information contained in the controller about a queue
	 */
	
	class ControllerQueue
	{
		std::string _name;				// Name of this queue
		KVFormat _format;

		std::vector<KVInfo> info;		// Information about this queue per server ( should be updated frequently )
		
	public:
		ControllerQueue( std::string name , KVFormat format )
		{
			_name = name;
			_format = format;
		}
							
		
		std::string str()
		{
			KVInfo total( info );

			
			std::ostringstream o;
			o << "(" << _format.str() << ") ";
			o << total.str();
			return o.str();		
		}
		
	};
	
	/**
	 Data manager at the controller
	 */
	class ControllerTask;
	
	class ControllerDataManager
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
		
		
	};

}

#endif