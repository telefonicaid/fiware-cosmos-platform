
#include <sys/stat.h>	// stat(.)
#include <stdarg.h>             /* va_start, va_arg, va_end                  */
#include <string.h>             // strchr
#include <sstream>              // std::ostringstream
#include <dirent.h>
#include <string>

#include "au/time.h"		// Own interface

namespace au
{
	int ellapsedSeconds(struct timeval* init_time)
	{
		struct timeval finish_time;
		gettimeofday(&finish_time, NULL);
		return finish_time.tv_sec - init_time->tv_sec;
	}
    
	std::string todayString()
	{
		return str_time( time(NULL) );
	}

	std::string str_time( time_t t )
	{
		
		struct tm timeinfo;
		char buffer_time[1024];
		
		localtime_r ( &t , &timeinfo );
		strftime (buffer_time,1024,"%d/%m/%Y (%X)",&timeinfo);
		
		return std::string( buffer_time );
	}
	
    
}
