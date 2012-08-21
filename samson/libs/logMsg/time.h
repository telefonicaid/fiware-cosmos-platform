

#ifndef LM_TIME_H_
#define LM_TIME_H_

#include <ctime>
#include <sstream>             // std::ostringstream
#include <stdio.h>             /* sprintf */
#include <string>              // std::string
#include <sys/time.h>          // struct timeval


namespace lm {
struct tm *gmtime_r(time_t *t, tm *tp);
}

#endif