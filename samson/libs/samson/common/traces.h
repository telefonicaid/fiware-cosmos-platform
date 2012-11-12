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
#ifndef TRACES_H
#define TRACES_H



/* ****************************************************************************
 *
 * logFd - file descriptor of log file for later manipulation
 */
extern int logFd;

namespace samson {


/* ****************************************************************************
*
* samsonInitTrace - initialize logging
*/
void samsonInitTrace
(
	int          argc,
	const char*  argv[],
	int*         logFdP,
	bool         tracesToStdout = false,
	bool         pidInName      = false
);

}

#endif
