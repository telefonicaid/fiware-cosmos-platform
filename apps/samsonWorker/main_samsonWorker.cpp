#include "parseArgs.h"          // parseArgs
#include "SamsonWorker.h"		// ss::SamsonWorker
#include "EndpointMgr.h"		// ss::EndpointMgr
#include "SamsonSetup.h"		// ss::SamsonSetup
#include "MemoryManager.h"		// ss::MemoryManager
#include "Endpoint.h"			// ss::Endpoint
#include "DiskManager.h"		// ss::DiskManager
#include "FileManager.h"		// ss::FileManager

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
bool             local;


#define S01 (long int) "samson01:1234"
/* ****************************************************************************
*
* parse arguments
*/
PaArgument paArgs[] =
{
	{ "-controller",  controller,  "CONTROLLER",  PaString,  PaOpt,   S01,   PaNL,   PaNL,  "controller IP:port"  },
	{ "-alias",       alias,       "ALIAS",       PaString,  PaReq,  PaND,   PaNL,   PaNL,  "alias"               },
	{ "-port",       &port,        "PORT",        PaShortU,  PaOpt,  1235,   1025,  65000,  "listen port"         },
	{ "-endpoints",  &endpoints,   "ENDPOINTS",   PaInt,     PaOpt,    80,      3,    100,  "number of endpoints" },
	{ "-workers",    &workers,     "WORKERS",     PaInt,     PaOpt,     1,      1,    100,  "number of workers"   },
	{ "-nolog",      &noLog,       "NO_LOG",      PaBool,    PaOpt, false,  false,   true,  "no logging"          },
	{ "-local",      &local,       "LOCAL",       PaBool,    PaOpt, false,  false,   true,  "local execution"     },

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
	paConfig("log to screen",                 (void*) "only errors");
	paConfig("log file line format",          (void*) "TYPE:DATE:EXEC-AUX/FILE[LINE] FUNC: TEXT");
	paConfig("screen line format",            (void*) "TYPE: TEXT");
	paConfig("log to file",                   (void*) true);

	paParse(paArgs, argC, (char**) argV, 1, false);// No more pid in the log file name
	lmAux((char*) "father");
	logFd = lmFirstDiskFileDescriptor();

	ss::SamsonSetup::shared();		// Load setup and create default directories
	ss::DiskManager::shared();		// Disk manager
	ss::FileManager::shared();		// File manager
	ss::MemoryManager::shared();	// Memory manager
	
	// Instance of network object and initialization
	// --------------------------------------------------------------------
	
	ss::Network networkP(endpoints,workers);
	networkP.init(ss::Endpoint::Worker, alias, port, controller);
	
	std::cout << "Waiting for network connection ...";
	networkP.runInBackground();
	while ( !networkP.ready() )
		sleep(1);
	std::cout << "OK\n";
	
	// This is only necessary when running multiple samsonworkers as separated process in the same machine
	if (local)
	{
		int worker_id = atoi(&alias[6]);
		ss::MemoryManager::shared()->setOtherSharedMemoryAsMarked(worker_id, workers);
	}
	
	// Instance of SamsonWormer object ( network contains at least the number of wokers )
	// -----------------------------------------------------------------------------------
	
	ss::SamsonWorker  *worker = new ss::SamsonWorker(&networkP);

	worker->touch();
	
	while (true)
		sleep(10000);

}
