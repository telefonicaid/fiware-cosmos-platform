
/* ****************************************************************************
*
* FILE            time.h
*
* AUTHOR          Andreu Urruela
*
* PROJECT         au library
*
* DATE            Septembre 2011
*
* DESCRIPTION
*
*      Usefull functions to work with time
*
* COPYRIGTH       Copyright 2011 Andreu Urruela. All rights reserved.
*
* ****************************************************************************/

#ifndef _H_AU_TIME
#define _H_AU_TIME


#include "logMsg/logMsg.h"     // LM_W
#include <sstream>             // std::ostringstream
#include <stdio.h>             /* sprintf */
#include <string>              // std::string
#include <sys/time.h>          // struct timeval

#include "au/ErrorManager.h"   // au::ErrorManager
#include "au/containers/map.h" // au::map




namespace au {
int ellapsedSeconds(struct timeval *init_time);

// String with today's timestamp
std::string todayString();


// conversions
double timeval_to_secs(timeval t);
timeval secs_to_timeval(double s);

size_t seconds_to_microseconds(double time);
size_t seconds_to_nanoseconds(double time);
}

#endif // ifndef _H_AU_TIME
