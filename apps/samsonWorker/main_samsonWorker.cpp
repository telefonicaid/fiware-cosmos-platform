#include "SamsonWorker.h"		// ss::SamsonWorker
#include "traces.h"				// LMT_*
#include "EndpointMgr.h"		// ss::EndpointMgr

/**
 Main routine for the samsonWorker
 */

int main(int argC, const char *argV[])
{
	ss::samsonInitTrace(argC, argV, true);
	
	ss::SamsonWorker  worker(argC, argV);
	ss::Network*      networkP;
	ss::EndpointMgr*  epMgr;

	networkP = new ss::Network();
	epMgr    = new ss::EndpointMgr(networkP, worker.endpoints, worker.workers);

	worker.endpointMgrSet(epMgr);
	worker.networkSet(networkP);
	
	worker.run();										// Run the object
}
