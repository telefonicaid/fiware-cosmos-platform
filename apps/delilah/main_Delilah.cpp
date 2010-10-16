
#include "Delilah.h"		//ss:Delilah
#include "traceLevels.h"        // LMT_*

int main(int argc, const char *argv[])
{
	LmStatus  s;
	char*     trace = (char*) "0-255";
	
	progName = lmProgName((char*) argv[0], 1, false);
	
	if ((s = lmPathRegister("/tmp/", "DEF", "DEF", NULL)) != LmsOk)
		EXIT(1, ("lmPathRegister: %s", lmStrerror(s)));
	if ((s = lmFdRegister(1, "TYPE: TEXT", "DEF", "controller", NULL)) != LmsOk)
		EXIT(1, ("lmPathRegister: %s", lmStrerror(s)));
	if  ((s = lmInit()) != LmsOk)
		EXIT(1, ("lmInit: %s", lmStrerror(s)));
	
	if ((argc >= 3) && (strcmp(argv[1], "-t") == 0))
		trace = (char*) argv[2];
	if ((s = lmTraceSet(trace)) != LmsOk)
		EXIT(1, ("lmTraceSet: %s", lmStrerror(s)));
	
	LM_F(("set trace levels to '%s'", trace));
	for (int ix = 0; ix < 256; ix++)
		LM_T(ix,  ("Testing trace level %d", ix));
	
	ss::Network network;						// Real network interface element

	ss::Delilah delilah( argc, argv , &network  );
	delilah.run();
}
