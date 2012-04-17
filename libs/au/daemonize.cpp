/* ****************************************************************************
*
* FILE                     daemonize.cpp
*
* AUTHOR                   Ken Zangelin
*
* CREATION DATE            Feb 25 2011
*
*/
#include <sys/types.h>          // pid_t
#include <sys/stat.h>           // umask
#include <unistd.h>             // getppid, fork, setsid
#include <stdio.h>              // stdin, stdout, stderr
#include <stdlib.h>             // free

#include "logMsg/logMsg.h"             // LM_*
#include "daemonize.h"          // Own interface


extern "C" void exit(int);
/* ****************************************************************************
*
* daemonize - 
*/
void daemonize(void)
{
	pid_t  pid;
	pid_t  sid;

	// already daemon
	if (getppid() == 1)
		return;

	pid = fork();
	if (pid == -1)
		LM_X(1, ("fork: %s", strerror(errno)));

	// Exiting father process
	if (pid > 0)
		exit(0);

	// Change the file mode mask */
	umask(0);

	// Removing the controlling terminal
	sid = setsid();
	if (sid == -1)
		LM_X(1, ("setsid: %s", strerror(errno)));

	// Change current working directory.
	// This prevents the current directory from being locked; hence not being able to remove it.
	if (chdir("/") == -1)
		LM_X(1, ("chdir: %s", strerror(errno)));

}

void deamonize_close_all(void)
{
	FILE*  s;
    
    // Redirect standard files to /dev/null
	s = freopen("/dev/null", "r", stdin);
	s = freopen("/dev/null", "w", stdout);
	s = freopen("/dev/null", "w", stderr);

}
