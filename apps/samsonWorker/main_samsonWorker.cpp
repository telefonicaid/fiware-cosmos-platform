#include "SamsonWorker.h"		// ss::SamsonWorker
#include "traces.h"				// LMT_*


/**
 Main routine for the samsonWorker
 */

int main(int argC, const char *argV[])
{
#if 0
	ss::SamsonWorker  worker(argC, argV);
	ss::Network*      networkP;
	ss::EndpointMgr*  epMgr;

	networkP = new ss::Network();
	epMgr    = new ss::EndpointMgr(networkP, worker.endpoints, worker.workers);

	worker.endpointMgrSet(epMgr);
	worker.networkSet(networkP);
	
	worker.run();
#endif
}
