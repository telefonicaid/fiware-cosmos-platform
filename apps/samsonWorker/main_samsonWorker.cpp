#include "parseArgs.h"          // parseArgs
#include "SamsonWorker.h"		// ss::SamsonWorker
#include "EndpointMgr.h"		// ss::EndpointMgr



/* ****************************************************************************
*
* Option variables
*/
unsigned short   port;
int              endpoints;
int              workers;
char             controller[80];
char             alias[36];
bool             noLog;



/* ****************************************************************************
*
* parse arguments
*/
PaArgument paArgs[] =
{
	{ "-controller",  controller,  "CONTROLLER",  PaString,  PaReq,  PaND,   PaNL,   PaNL,  "controller IP:port"  },
	{ "-alias",       alias,       "ALIAS",       PaString,  PaReq,  PaND,   PaNL,   PaNL,  "alias"               },
	{ "-port",       &port,        "PORT",        PaShortU,  PaReq,  PaND,   1025,  65000,  "listen port"         },
	{ "-endpoints",  &endpoints,   "ENDPOINTS",   PaInt,     PaOpt,    80,      3,    100,  "number of endpoints" },
	{ "-workers",    &workers,     "WORKERS",     PaInt,     PaOpt,     5,      1,    100,  "number of workers"   },
	{ "-nolog",      &noLog,       "NO_LOG",      PaBool,    PaOpt, false,  false,   true,  "no logging"          },

	PA_END_OF_ARGS
};



/* ****************************************************************************
*
* logFd - file descriptor for log file used in all libraries
*/
namespace ss
{
	int logFd = -1;
}



/* ****************************************************************************
*
* main - 
*/
int main(int argC, const char *argV[])
{
	paConfig("prefix",                        (void*) "SSW_");
	paConfig("log to file",                   (void*) "/tmp/");
	paConfig("usage and exit on any warning", (void*) true);
	paConfig("log to screen",                 (void*) "only errors");
	paConfig("log file line format",          (void*) "TYPE:DATE:EXEC-AUX/FILE[LINE] FUNC: TEXT");
	paConfig("screen line format",            (void*) "TYPE: TEXT");

	paParse(paArgs, argC, (char**) argV, 1, true);
	lmAux((char*) "father");


	ss::SamsonWorker  worker(controller, alias, port, workers, endpoints);
	ss::Network*      networkP;
	ss::EndpointMgr*  epMgr;

	
	networkP = new ss::Network();
	epMgr    = new ss::EndpointMgr(networkP, endpoints, workers);

	worker.endpointMgrSet(epMgr);
	worker.networkSet(networkP);
	
	worker.run();										// Run the object
}
