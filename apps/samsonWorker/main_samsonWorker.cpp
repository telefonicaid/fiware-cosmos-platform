#include "SamsonWorker.h"		// ss::SamsonWorker
#include "traces.h"				// LMT_*


/**
 Main routine for the samsonWorker
 */

int main(int argC, const char *argV[])
{
	ss::SamsonWorker  worker(argC, argV);
	ss::Network*      networkP;

	networkP = new ss::Network(worker.endpoints, worker.workers);

	worker.networkSet(networkP);
	worker.run();
}
