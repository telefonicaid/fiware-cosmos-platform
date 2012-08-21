
#include <dirent.h>
#include <math.h>
#include <sstream>              // std::ostringstream
#include <stdarg.h>             /* va_start, va_arg, va_end                  */
#include <string.h>             // strchr
#include <string>
#include <sys/stat.h>   // stat(.)

#include "au/string.h"
#include "au/time.h"


namespace au {
int ellapsedSeconds(struct timeval *init_time) {
  struct timeval finish_time;

  gettimeofday(&finish_time, NULL);
  return finish_time.tv_sec - init_time->tv_sec;
}

std::string todayString() {
  return str_time(time(NULL));
}

double timeval_to_secs(timeval t) {
  return (double)t.tv_sec + ((double)t.tv_usec / 1000000.0);
}

timeval secs_to_timeval(double s) {
  // Check negative numbers ( just in case )
  if (s <= 0)
    s = 0; timeval tv;

  tv.tv_sec  = (long)fabs(s);
  s -= tv.tv_sec;

  tv.tv_usec = s * 1000000;
  return tv;
}

size_t seconds_to_microseconds(double time) {
  return time * 1000000;
}

size_t seconds_to_nanoseconds(double time) {
  return time * 1000000000;
}
}

