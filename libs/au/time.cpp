
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
}
