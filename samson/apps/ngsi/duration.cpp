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
*  FILE                 duration.cpp -  
*
*
*
*
*/
#include <string.h>                     // strcmp, ...
#include <string>                       // std::string

#include "logMsg/logMsg.h"

#include "traceLevels.h"                // Trace levels for log msg library

using namespace std;



/* ****************************************************************************
*
* durationStringToSeconds - 
*/
long durationStringToSeconds(std::string durationString)
{
	char*        start;
	char         initially[128];
	char*        duration  = (char*) durationString.c_str();
	int          sign      = 1;
	int          years     = 0;
	int          months    = 0;
	int          days      = 0;
	int          hours     = 0;
	int          minutes   = 0;
	int          seconds   = 0;

	LM_T(LmtDuration, ("Incoming duration '%s'", durationString.c_str()));
	strcpy(initially, duration);

	if (duration[0] == '-')
	{
		sign = -1;
		++duration;
	}

	if (*duration != 'P')
		return -1;
	++duration;


	// 
	// PT - 'special case' ...
	//
	if ((*duration == 'T') && (duration[3] != ':'))
	{
	   ++duration;
	}



	//
	// Find time string
	//
	char* T = strstr(duration, "T");
	if (T != NULL)
	{
		*T = 0;
		++T;
		
		if ((T[2] != ':') || (T[5] != ':'))
		{
			LM_E(("Colon not found where expected"));
			return -1;
		}
		T[2] = 0;
		T[5] = 0;

		hours   = atoi(T);
		minutes = atoi(&T[3]);
		seconds = atoi(&T[6]);
	}


	//
	// Now parse the rest
	//
	while (*duration != 0)
	{
		start = duration;
		while (isdigit(*duration))
			++duration;
		if (*duration == 'Y')
		{
			*duration = 0;
			++duration;
			years = atoi(start);
		}
		else if (*duration == 'M')
		{
			*duration = 0;
			++duration;
			months = atoi(start);
		}
		else if (*duration == 'D')
		{
			*duration = 0;
			++duration;
			days = atoi(start);
		}
		else
		{
			LM_E(("Unrecognized character found in duration string: '%c'", *duration));
			return -1;
		}
	}

	LM_T(LmtDuration, ("Parsed duration '%s'", initially));
	LM_T(LmtDuration, ("o Sign:     %d", sign));
	LM_T(LmtDuration, ("o Years:    %d", years));
	LM_T(LmtDuration, ("o Months:   %d", months));
	LM_T(LmtDuration, ("o Days:     %d", days));
	LM_T(LmtDuration, ("o Hours:    %d", hours));
	LM_T(LmtDuration, ("o Minutes:  %d", minutes));
	LM_T(LmtDuration, ("o Seconds:  %d", seconds));

	long secs = seconds + 60 * minutes + 3600 * hours + (3600 * 24) * days + (3600 * 24 * 30) * months + (3600 * 24 * 365) * years;

	secs *= sign;

	LM_T(LmtDuration, ("o Result:   %lu", secs));
	return secs;
}
