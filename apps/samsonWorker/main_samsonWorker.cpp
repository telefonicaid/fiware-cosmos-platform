#include "SamsonWorker.h"		// ss::SamsonWorker
#include "traces.h"				// LMT_*


/**
 Main routine for the samsonWorker
 */

int main(int argc, const char *argv[])
{
	ss::samsonInitTrace(argc , argv, true);
	
	ss::Network      network;   // Real network element
	ss::SamsonWorker worker(argc, argv, &network);

	worker.run();
}
