
#ifndef _H_QUEUE
#define _H_QUEUE

#include "KVInfo.h"						// ss::KVInfo
#include "MonitorParameter.h"			// ss::MonitorBlock

namespace ss {
	
	/**
	 Information contained in the controller about a queue
	 */
	
	class Queue
	{
		std::string _name;				// Name of this queue
		KVFormat _format;				// Format of the queue
		KVInfo _info;					// Information about this queue
		
		MonitorBlock monitor;			// Set of parameters to monitor for this queue

		friend class Monitor;
		
	public:
		Queue( std::string name , KVFormat format )
		{
			_name = name;
			_format = format;
		}
		
		KVFormat format()
		{
			return _format;
		}
		KVInfo info()
		{
			return _info;
		}
		
		void addFile( int worker, std::string _fileName , size_t _size , size_t _kvs )
		{
			_info.size += _size;
			_info.kvs += _kvs;
		}
		
		
		std::string getStatus()
		{
			std::ostringstream o;
			o << _name << "(" << _format.str() << ") " << _info.str();
			return o.str();		
		}
	};
}

#endif