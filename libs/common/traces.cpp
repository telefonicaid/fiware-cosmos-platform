
#include "traces.h"			// Own interface
#include "CommandLine.h"	// au::CommandLine
#include "logMsg.h"			// Log message
#include "Macros.h"			// EXIT

namespace ss {
	
	void samsonInitTrace( int argc, const char *argv[] )
	{
		au::CommandLine commandLine;
		commandLine.set_flag_string("t", "");
		
		commandLine.parse(argc, argv);

		std::string trace = commandLine.get_flag_string("t");
		
		LmStatus  s;
		
		progName = lmProgName((char*) argv[0], 1, false);
		
		if ((s = lmPathRegister("/tmp/", "DEF", "DEF", NULL)) != LmsOk)
			EXIT(1, ("lmPathRegister: %s", lmStrerror(s)));
		if ((s = lmFdRegister(1, "TYPE: TEXT", "DEF", "controller", NULL)) != LmsOk)
			EXIT(1, ("lmPathRegister: %s", lmStrerror(s)));
		if  ((s = lmInit()) != LmsOk)
			EXIT(1, ("lmInit: %s", lmStrerror(s)));
		
		if ((s = lmTraceSet((char*)trace.c_str())) != LmsOk)
			EXIT(1, ("lmTraceSet: %s", lmStrerror(s)));
		
		/*
		 LM_F(("set trace levels to '%s'", trace));
		 for (int ix = 0; ix < 256; ix++)
		 LM_T(ix,  ("Testing trace level %d", ix));
		 */
	}

}
