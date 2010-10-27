#ifndef _H_CONTROLLER_QUEUE
#define _H_CONTROLLER_QUEUE


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
}

#endif
