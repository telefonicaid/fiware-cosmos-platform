#ifndef _H_CONTROLLER_DATA_MANAGER
#define _H_CONTROLLER_DATA_MANAGER

#include "Lock.h"				// au::Lock
#include <sstream>				// std::ostringstream
#include "samson/KVFormat.h"	// ss:: KVFormat
#include "Format.h"				// au::Format

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
		
		int worker_creation_counter;	// Servers that has confirmed the "creation of this queue" 
		
	public:
		ControllerQueue( std::string name , KVFormat format )
		{
			_name = name;
			_format = format;
			worker_creation_counter = 0;
		}
							
		
		std::string str()
		{
			KVInfo total( info );

			
			std::ostringstream o;
			o << "[W"<< worker_creation_counter <<" ] ";
			o << "(" << _format.str() << ") ";
			o << total.str();
			return o.str();		
		}
		
	};
	
	/**
	 Data manager at the controller
	 */
	 
	
	class ControllerDataManager
	{
		au::Lock lock;
		std::map< std::string , ControllerQueue*> queue;	// Queues in the system
		
	public:
		
		
		/**
		 Add a queue to the system
		 */
		
		void addQueue( std::string name , KVFormat format )
		{
			lock.lock();
			
			ControllerQueue *tmp = new ControllerQueue(name , format);
			
			// TODO: Send a message to each worker to create this queue too
			
			queue.insert( std::pair< std::string , ControllerQueue*>( name , tmp ) );
			lock.unlock();
		}
		
		
		
		/**
		 Get a string describing status
		 Usefull for debuggin
		 */
		
		std::string status()
		{
			std::ostringstream o;
			
			lock.lock();
			for ( std::map< std::string , ControllerQueue*>::iterator q = queue.begin() ; q != queue.end() ; q++)
				o << q->first << " " << q->second->str() << std::endl;
			lock.unlock();
			
			return o.str();
		
		}
		
	};

}

#endif