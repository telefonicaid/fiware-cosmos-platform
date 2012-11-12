/*
 * Telefónica Digital - Product Development and Innovation
 *
 * THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND,
 * EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
 *
 * Copyright (c) Telefónica Investigación y Desarrollo S.A.U.
 * All rights reserved.
 */

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


#include <stdio.h>             /* sprintf */
#include <string>				// std::string
#include <sys/time.h>           // struct timeval
#include <sstream>              // std::ostringstream
#include "logMsg/logMsg.h"             // LM_W

#include "au/ErrorManager.h"        // au::ErrorManager
#include "au/containers/map.h"                 // au::map

#include "au/namespace.h"


NAMESPACE_BEGIN(au)


int ellapsedSeconds(struct timeval *init_time);    

// String with today's timestamp
std::string todayString();


// conversions
double timeval_to_secs( timeval t );
timeval secs_to_timeval( double s );

size_t seconds_to_microseconds( double time );
size_t seconds_to_nanoseconds( double time );


NAMESPACE_END

#endif
