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
#include "logMsg/logMsg.h"			// Log message

#include "au/CommandLine.h"	// au::CommandLine
#include "samson/common/Macros.h"			// EXIT
#include "samson/common/traces.h"			// Own interface



namespace samson {
	


/* ****************************************************************************
*
* logFd - file descriptor of log file for later manipulation
*/
// int logFd;



/* ****************************************************************************
*
* samsonInitTrace - 
*/
void samsonInitTrace
(
	int          argc,
	const char*  argv[],
	int*         logFdP,
	bool         tracesToStdout,
	bool         pidInName
)
{
	LmStatus  s;
	int       logFdIndex;

	au::CommandLine commandLine;
	commandLine.set_flag_string("t", "");

	commandLine.parse(argc, argv);

	std::string trace = commandLine.get_flag_string("t");

	progName = lmProgName((char*) argv[0], 1, pidInName);
		
	if ((s = lmPathRegister("/tmp/", "TYPE:DATE:EXEC-AUX/FILE[LINE] FUNC: TEXT", "DEF", &logFdIndex)) != LmsOk)
		EXIT(1, ("lmPathRegister: %s", lmStrerror(s)));

	if (tracesToStdout)
	{
	   if ((s = lmFdRegister(1, "TYPE: TEXT", "DEF", "controller", NULL)) != LmsOk)
		  EXIT(1, ("lmPathRegister: %s", lmStrerror(s)));
	}

	if  ((s = lmInit()) != LmsOk)
		EXIT(1, ("lmInit: %s", lmStrerror(s)));
		
	if ((s = lmTraceSet((char*)trace.c_str())) != LmsOk)
		EXIT(1, ("lmTraceSet: %s", lmStrerror(s)));

	lmAux((char*) "father");

	if (logFdP != NULL)
	{
		lmFdGet(logFdIndex, logFdP);
		//printf("logFd: %d", *logFdP);
	}
}
}
