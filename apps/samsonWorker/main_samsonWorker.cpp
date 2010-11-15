#include "SamsonWorker.h"		// ss::SamsonWorker
#include "traces.h"				// LMT_*
#include "EndpointMgr.h"		// ss::EndpointMgr



/* ****************************************************************************
*
* logFd - file descriptor for log file used in all libraries
*/
namespace ss
{
int logFd = -1;
}


/**
 Main routine for the samsonWorker
 */

int main(int argC, const char *argV[])
{
	ss::SamsonWorker  worker;
	ss::Network*      networkP;
	ss::EndpointMgr*  epMgr;

	worker.logInit(argV[0]);
	worker.parseArgs(argC, argV);
	
	networkP = new ss::Network();
	epMgr    = new ss::EndpointMgr(networkP, worker.endpoints, worker.workers);

	worker.endpointMgrSet(epMgr);
	worker.networkSet(networkP);
	
	worker.run();										// Run the object
}
