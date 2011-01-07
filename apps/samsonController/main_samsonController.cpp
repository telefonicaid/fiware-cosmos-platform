#include "parseArgs.h"          // parseArgs
#include "samsonDirectories.h"  // SAMSON_SETUP_FILE
#include "SamsonController.h"	// ss::SamsonController
#include "SamsonSetup.h"		// ss::SamsonSetup
#include "MemoryManager.h"		// ss::MemoryManager
#include "DiskManager.h"		// ss::DiskManager
#include "FileManager.h"		// ss::FileManager
#include "LockDebugger.h"       // au::LockDebugger

/* ****************************************************************************
*
* Option variables
*/
unsigned short   port;
int              endpoints;
int              workers;
char			 workingDir[1024]; 	


/* ****************************************************************************
*
* parse arguments
*/
PaArgument paArgs[] =
{
	{ "-working",     workingDir,  "WORKING",     PaString,  PaOpt,  _i SAMSON_DEFAULT_WORKING_DIRECTORY,   PaNL,   PaNL,  "Working directory" },
	{ "-port",       &port,        "PORT",        PaShortU,  PaOpt,                  1234,   1025,  65000,  "listen port"         },
	{ "-endpoints",  &endpoints,   "ENDPOINTS",   PaInt,     PaOpt,                    80,      3,    100,  "number of endpoints" },
	{ "-workers",    &workers,     "WORKERS",     PaInt,     PaOpt,                     5,      1,    100,  "number of workers"   },

	PA_END_OF_ARGS
};



/* ****************************************************************************
*
* logFd - file descriptor for log file used in all libraries
*/
int logFd = -1;



/* ****************************************************************************
*
* main - main routine for the samsonController
*/
int main(int argC, const char* argV[])
{
	paConfig("prefix",                        (void*) "SSC_");
	paConfig("usage and exit on any warning", (void*) true);
	paConfig("log to screen",                 (void*) "only errors");
	paConfig("log file line format",          (void*) "TYPE:DATE:EXEC/FILE[LINE] FUNC: TEXT");
	paConfig("screen line format",            (void*) "TYPE: TEXT");
	paConfig("log to file",                   (void*) true);

	paParse(paArgs, argC, (char**) argV, 1, false);

	au::LockDebugger::shared();    // Debuggin of Lock usage ( necessary here where it is only one thread )

	ss::SamsonSetup::load( workingDir );		// Load setup and create all directories
	
	ss::DiskManager::shared();		// Disk manager
	ss::MemoryManager::shared();	// Memory manager

	
	
	
	// Instance of network object and initialization
	// --------------------------------------------------------------------
	
	ss::Network networkP(endpoints,workers);
	networkP.initAsSamsonController(port, workers);
	networkP.runInBackground();
	
	
	// Instance of the samson controller	
	// --------------------------------------------------------------------
	
	ss::SamsonController  controller(&networkP);
	controller.runBackgroundProcesses();
	controller.touch();
	
	while(true)
		sleep(10000);
}
