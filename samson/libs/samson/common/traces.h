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
