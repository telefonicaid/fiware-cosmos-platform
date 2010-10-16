


#include "SamsonController.h"	// ss::SamsonController
#include "traceLevels.h"        // LMT_*


/* ****************************************************************************
 *
 * main - main routine for the samsonController
 */
#define LOG_FORMAT "TYPE:EXEC:FILE[LINE]:FUNC: TEXT"
int main(int argc, const char* argv[])
{
	LmStatus  s;
	
	progName = lmProgName((char*) argv[0], 1, false);
	
	if ((s = lmPathRegister("/tmp/", LOG_FORMAT, "DEF", NULL)) != LmsOk)
		EXIT(1, ("lmPathRegister: %s", lmStrerror(s)));
	if ((s = lmFdRegister(1, "TYPE: TEXT", "DEF", "controller", NULL)) != LmsOk)
		EXIT(1, ("lmPathRegister: %s", lmStrerror(s)));
	if  ((s = lmInit()) != LmsOk)
		EXIT(1, ("lmInit: %s", lmStrerror(s)));
	
	ss::Network network;		// Real network element
	ss::SamsonController controller(argc, argv , &network);
	controller.run();
}
