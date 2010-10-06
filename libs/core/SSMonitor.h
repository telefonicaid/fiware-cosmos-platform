#ifndef SAMSON_MONITOR_H
#define SAMSON_MONITOR_H

#include <boost/circular_buffer.hpp>
#include <fstream>
#include <sstream>
#include <string>
#include <map>
#include <iostream>

#include "ParameterMonitor.h"
#include "Lock.h"                /* Lock                            */


namespace ss {

	class KVSet;
	class KVQueue;
	
	class SSMonitor
	{
		au::Lock lock;
		
		ParameterMonitor *random;
		ParameterMonitor *cores;

		std::map<std::string,ParameterMonitor*> parameters;
		std::map<std::string, KVQueue*> queues;					// Only for monitoring		
		
		SSMonitor();
		~SSMonitor();
		
	public:
		static SSMonitor* shared();		
		std::string get( );

	private:
		std::string _getQueuePlot(std::string parameter);
		std::string _getSystemPlot( );
		std::string _getData( std::string label , std::string parameter , int scale );
		std::string _getValues( std::string name , int scale );

	public:
		// Function called by thread
		void run();
		
		void addQueueToMonitor( KVQueue *queue );
		void removeQueueToMonitor( KVQueue *queue );
		
	private:
		KVQueue * findKVQueue( std::string queue_name );
	};
}

#endif
