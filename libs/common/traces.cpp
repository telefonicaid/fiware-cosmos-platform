#include "CommandLine.h"	// au::CommandLine
#include "logMsg.h"			// Log message
#include "Macros.h"			// EXIT

#include "traces.h"			// Own interface



namespace ss {
	
void samsonInitTrace(int argc, const char *argv[], bool pidInName)
{
	LmStatus  s;

	au::CommandLine commandLine;
	commandLine.set_flag_string("t", "");

	commandLine.parse(argc, argv);

	std::string trace = commandLine.get_flag_string("t");

	progName = lmProgName((char*) argv[0], 1, pidInName);
		
	if ((s = lmPathRegister("/tmp/", "DEF", "DEF", NULL)) != LmsOk)
		EXIT(1, ("lmPathRegister: %s", lmStrerror(s)));
	if ((s = lmFdRegister(1, "TYPE: TEXT", "DEF", "controller", NULL)) != LmsOk)
		EXIT(1, ("lmPathRegister: %s", lmStrerror(s)));
	if  ((s = lmInit()) != LmsOk)
		EXIT(1, ("lmInit: %s", lmStrerror(s)));
		
	if ((s = lmTraceSet((char*)trace.c_str())) != LmsOk)
		EXIT(1, ("lmTraceSet: %s", lmStrerror(s)));
	}
}
