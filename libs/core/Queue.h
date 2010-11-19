
#ifndef _H_QUEUE
#define _H_QUEUE

#include "KVInfo.h"						// ss::KVInfo
#include "MonitorParameter.h"			// ss::MonitorBlock
#include <list>							// std::list
#include "samson/KVFormat.h"			// KVFormat

namespace ss {
	
	class QueueFile;
	
	/**
	 Information contained in the controller about a queue
	 */
	
	class Queue
	{
		std::string _name;				// Name of this queue
		KVFormat _format;				// Format of the queue
		KVInfo _info;					// Information about this queue

		/**
		 Monitoring system
		 */
		
		friend class Monitor;
		MonitorBlock monitor;			// Set of parameters to monitor for this queue

		std::list< QueueFile* > files;	// List of files
		
	public:
		
		Queue( std::string name , KVFormat format )
		{
			_name = name;
			_format = format;
		}
		
		std::string getName(){ return _name; }
		KVFormat format() { return _format; }
		KVInfo info() { return _info; }

		/**
		 Main functions to add files to this queue
		 */
		void addFile( int worker, std::string _fileName , KVInfo info );
		
		std::string getStatus()
		{
			std::ostringstream o;
			o << _name << "(" << _format.str() << ") [ " << files.size() << " files ] " << _info.str();
			return o.str();		
		}
	};
}

#endif