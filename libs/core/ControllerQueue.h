#ifndef _H_CONTROLLER_QUEUE
#define _H_CONTROLLER_QUEUE

#include "KVInfo.h"							// ss::KVInfo


namespace ss {
	
	/**
	 Information contained in the controller about a queue
	 */
	
	class ControllerQueue
	{
		std::string _name;				// Name of this queue
		KVFormat _format;				// Format of the queue
		KVInfo _info;					// Information about this queue
		
		
	public:
		ControllerQueue( std::string name , KVFormat format )
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
		
		std::string getStatus()
		{
			std::ostringstream o;
			o << _name << "(" << _format.str() << ") " << _info.str();
			return o.str();		
		}
	};
}

#endif
