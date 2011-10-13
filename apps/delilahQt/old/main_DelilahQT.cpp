#include "parseArgs.h"          // parseArgs
#include "Delilah.h"		    // ss::Delilah



/* ****************************************************************************
*
* Option variables
*/
int              endpoints;
int              workers;
char             controller[80];



#define NO "no_controller"
/* ****************************************************************************
*
* parse arguments
*/
PaArgument paArgs[] =
{
	{ "-controller",  controller,  "CONTROLLER",  PaString,  PaOpt, _i NO,   PaNL,   PaNL,  "controller IP:port"  },
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

	paParse(paArgs, argC, (char**) argV, 1, false);
	lmAux((char*) "father");
	logFd = lmFirstDiskFileDescriptor();

	ss::Network  network;
	ss::Delilah delilah(&network, argC, argV, controller, endpoints, workers);
	DelilahClient *client = new DelilahQt(delilah);
	delilah->setClient( client );

	delilah.run();
}