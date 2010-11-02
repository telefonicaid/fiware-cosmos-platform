#ifndef TRACES_H
#define TRACES_H



namespace ss {



/* ****************************************************************************
*
* logFd - file descriptor of log file for later manipulation
*/
extern int logFd;



/* ****************************************************************************
*
* samsonInitTrace - initialize logging
*/
void samsonInitTrace( int argc, const char *argv[], bool pidInName = false);

}

#endif
