#include "SamsonController.h"	// ss::SamsonController
#include "traces.h"				// LMT_*



/* ****************************************************************************
*
* logFd - file descriptor for log file used in all libraries
*/
namespace ss
{
int logFd = -1;
}



/* ****************************************************************************
*
* main - main routine for the samsonController
*/
int main(int argc, const char* argv[])
{
	ss::samsonInitTrace(argc, argv, &ss::logFd, true);

	ss::Network network;		// Real network element
	ss::SamsonController controller(argc, argv, &network);

	controller.run();
}
