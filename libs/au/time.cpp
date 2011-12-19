
#include <sys/stat.h>	// stat(.)
#include <stdarg.h>             /* va_start, va_arg, va_end                  */
#include <string.h>             // strchr
#include <sstream>              // std::ostringstream
#include <dirent.h>
#include <string>
#include <math.h>

#include "au/time.h"		// Own interface


NAMESPACE_BEGIN(au)

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

double timeval_to_secs( timeval t )
{
    return (double) t.tv_sec + ((double)t.tv_usec / 1000000.0);
}

timeval secs_to_timeval( double s )
{
    // Check negative numbers ( just in case )
    if( s <= 0)
        s=0;
    
    timeval tv;
    
    tv.tv_sec  = (long) fabs(s);
    s -= tv.tv_sec;
    
    tv.tv_usec = s*1000000;
    return tv;
}

size_t seconds_to_microseconds( double time )
{
    return time*1000000;
}

size_t seconds_to_nanoseconds( double time )
{
    return time*1000000000;
}


NAMESPACE_END

