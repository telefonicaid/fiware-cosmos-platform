

#ifndef LM_TIME_H_
#define LM_TIME_H_

#include <stdio.h>             /* sprintf */
#include <string>				// std::string
#include <sys/time.h>           // struct timeval
#include <sstream>              // std::ostringstream
#include <ctime>    


namespace lm {
    struct tm *gmtime_r (time_t *t, tm *tp);
}

#endif