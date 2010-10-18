#include "SamsonWorker.h"		// ss::SamsonWorker
#include "traceLevels.h"        // LMT_*



/**
 Main routine for the samsonWorker
 */

int main(int argc, const char *argv[])
{
	LmStatus  s;
	
	progName = lmProgName((char*) argv[0], 1, true);
	
	if ((s = lmPathRegister("/tmp/", "DEF", "DEF", NULL)) != LmsOk)
		EXIT(1, ("lmPathRegister: %s", lmStrerror(s)));
	if ((s = lmFdRegister(1, "TYPE: TEXT", "DEF", "controller", NULL)) != LmsOk)
		EXIT(1, ("lmPathRegister: %s", lmStrerror(s)));
	if  ((s = lmInit()) != LmsOk)
		EXIT(1, ("lmInit: %s", lmStrerror(s)));
	
	ss::Network      network;   // Real network element
	ss::SamsonWorker worker(argc, argv, &network);

	worker.run();
}
