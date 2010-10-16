
#ifndef _H_DEVICE_DISK_ACCESS_STATISTICS
#define _H_DEVICE_DISK_ACCESS_STATISTICS

#include <stdio.h>			// fopen , fclose, ...
#include <string>			// std::string
#include <map>				// std::map
#include <set>				// std::set
#include <list>				// std::list
#include "Lock.h"			// au::Lock
#include "StopLock.h"		// au::StopLock
#include <pthread.h>        /* pthread_mutex_init, ...                  */
#include <sys/stat.h>		// stat(.)
#include <iostream>			// std::cout
#include <sstream>			// std::ostringstream
#include <Format.h>			// au::Format
#include <time.h>			// clock(.)


namespace ss {
	
	class DeviceDiskAccessStatistics
	{
		size_t size;
		double time;
		
		size_t average_size;
		double average_time;
		
	public:
		
		DeviceDiskAccessStatistics()
		{
			size = 0;
			time = 0;
			average_size = 0;
			average_time = 0;
		}
		
		void addSample( size_t _size, double _time )
		{
			size += _size;
			time += _time;
			
			if( average_size == 0)
			{
				average_size = _size;
				average_time = _time;
			}
			else
			{
				average_size = 0.8*average_size + 0.2*size;
				average_time = 0.8*average_time + 0.2*time;
			}
			
		}
		
		size_t getRate()
		{
			if( size == 0)
				return 0;
			else
				return ( size / time );
		}
		
		size_t getAverageRate()
		{
			if( average_size == 0)
				return 0;
			else
				return ( average_size / average_time );
		}
		
		std::string str()
		{
			std::ostringstream o;
			o << "S:" << au::Format::string( size ) << " T:" << au::Format::time_string( time ) << "("<<  au::Format::string( getRate()) << ")";
			//o << "S:" << au::Format::string( average_size ) << " T:" << au::Format::time_string( average_time ) << "("<<  au::Format::string( getAverageRate()) << ")";
			return o.str();
		}
	};
}

#endif
