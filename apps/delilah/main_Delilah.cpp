#include "Delilah.h"		//ss:Delilah
#include "traces.h"			// LMT_*

/* ****************************************************************************
*
* logFd - file descriptor for log file used in all libraries
*/
namespace ss
{
int logFd = -1;
}



int main(int argc, const char *argv[])
{
	ss::samsonInitTrace(argc, argv, &ss::logFd);

	ss::Network network;								// Real network interface element
	ss::Delilah delilah( argc, argv , &network);		// Delilah instance
	delilah.run();										// Main run loop
}
