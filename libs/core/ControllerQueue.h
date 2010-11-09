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
		
		bool ready;						// Read means that it has been created in all queues

		// Information about "block"...
		
	public:
		ControllerQueue( std::string name , KVFormat format )
		{
			_name = name;
			_format = format;
			ready = false;
		}
		
		void setReady()
		{
			ready = true;
		}
		
		KVFormat format()
		{
			return _format;
		}
		KVInfo info()
		{
			return _info;
		}
		
		std::string str()
		{
			std::ostringstream o;
			o << "(" << _format.str() << ") ";
			if( !ready )
				o << "Not ready";
			return o.str();		
		}
	};
}

#endif
