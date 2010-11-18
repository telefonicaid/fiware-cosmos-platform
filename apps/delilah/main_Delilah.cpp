#include "parseArgs.h"          // parseArgs
#include "Delilah.h"		    // ss::Delilah



/* ****************************************************************************
*
* Option variables
*/
int              endpoints;
int              workers;
char             controller[80];
bool             console;
bool             basic;



#define NO "no_controller"
/* ****************************************************************************
*
* parse arguments
*/
PaArgument paArgs[] =
{
	{ "-controller",  controller,  "CONTROLLER",  PaString,  PaOpt, _i NO,   PaNL,   PaNL,  "controller IP:port"  },
	{ "-console",    &console,     "CONSOLE",     PaBool,    PaOpt, false,  false,   true,  "console mode"        },
	{ "-basic",      &basic,       "BASIC",       PaBool,    PaOpt, false,  false,   true,  "basic mode"          },
	{ "-endpoints",  &endpoints,   "ENDPOINTS",   PaInt,     PaOpt,    80,      3,    100,  "number of endpoints" },
	{ "-workers",    &workers,     "WORKERS",     PaInt,     PaOpt,     5,      1,    100,  "number of workers"   },

	PA_END_OF_ARGS
};



/* ****************************************************************************
*
* logFd - file descriptor for log file used in all libraries
*/
int logFd = -1;



/* ****************************************************************************
*
* main - 
*/
int main(int argC, const char *argV[])
{
	paConfig("prefix",                        (void*) "SSW_");
	paConfig("usage and exit on any warning", (void*) true);
	paConfig("log to screen",                 (void*) false);
	paConfig("log file line format",          (void*) "TYPE:DATE:EXEC-AUX/FILE[LINE] FUNC: TEXT");
	paConfig("log to file",                   (void*) true);

	paParse(paArgs, argC, (char**) argV, 1, true);
	lmAux((char*) "father");
	logFd = lmFirstDiskFileDescriptor();

	LM_M(("reads:   '%s'", BA_FT(lmReads)));
	LM_M(("writes:  '%s'", BA_FT(lmWrites)));
	LM_M(("verbose: '%s'", BA_FT(lmVerbose)));
	LM_M(("debug:   '%s'", BA_FT(lmDebug)));

	LM_M(("logFd: %d", logFd));

	ss::Network  network;
	ss::Delilah  delilah(&network, argC, argV, controller, endpoints, workers, console, basic);

	delilah.run();
}
