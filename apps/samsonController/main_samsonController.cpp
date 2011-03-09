/* ****************************************************************************
*
* FILE                     main_samsonController.cpp
*
* AUTHOR                   Ken Zangelin
*
* CREATION DATE            Dec 14 2010
*
*/
#include <unistd.h>             // read
#include <fcntl.h>              // open, O_RDONLY, ...
#include <sys/stat.h>           // struct stat

#include "parseArgs.h"          // parseArgs

#include "ports.h"              // CONTROLLER_PORT
#include "samsonDirectories.h"  // SAMSON_PLATFORM_PROCESSES
#include "SamsonController.h"	// ss::SamsonController
#include "SamsonSetup.h"		// ss::SamsonSetup
#include "platformProcesses.h"  // ss::platformProcessesGet, ss::platformProcessesSave
#include "MemoryManager.h"		// ss::MemoryManager
#include "DiskManager.h"		// ss::DiskManager
#include "FileManager.h"		// ss::FileManager
#include "LockDebugger.h"       // au::LockDebugger



/* ****************************************************************************
*
* Option variables
*/
int              endpoints;
char			 workingDir[1024];



#define DEF_WD   _i SAMSON_DEFAULT_WORKING_DIRECTORY
#define DEF_WF   _i SAMSON_PLATFORM_PROCESSES
/* ****************************************************************************
*
* parse arguments
*/
PaArgument paArgs[] =
{
	{ "-working",     workingDir,    "WORKING",                  PaString, PaOpt, DEF_WD,  PaNL,  PaNL, "working directory"       },
	{ "-endpoints",  &endpoints,     "ENDPOINTS",                PaInt,    PaOpt,     80,     3,   100, "number of endpoints"     },

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
	ss::ProcessVector*  processVec;
	int                 processVecSize;

	paConfig("prefix",                        (void*) "SSC_");
	paConfig("usage and exit on any warning", (void*) true);
	paConfig("log to screen",                 (void*) "only errors");
	paConfig("log file line format",          (void*) "TYPE:DATE:EXEC/FILE[LINE] FUNC: TEXT");
	paConfig("screen line format",            (void*) "TYPE:EXEC: TEXT");
	paConfig("log to file",                   (void*) true);

	paParse(paArgs, argC, (char**) argV, 1, false);

	LM_T(LmtInit, ("Started with arguments:"));
	for (int ix = 0; ix < argC; ix++)
		LM_T(LmtInit, ("  %02d: '%s'", ix, argV[ix]));

	processVec = ss::platformProcessesGet(&processVecSize);
	if (processVec == NULL)
		LM_X(1, ("Error retreiving info about the platform processes - can't function without it!"));

	LM_T(LmtInit, ("%d workers", processVec->processes - 1));

	au::LockDebugger::shared();         // Lock usage debugging (necessary here where there is only one thread)
	ss::SamsonSetup::load(workingDir);  // Load setup and create all directories
	ss::DiskManager::shared();          // Disk manager
	ss::MemoryManager::init();          // Memory manager
	ss::ModulesManager::init();			// Init the modules manager
	
	
	
	// Instance of network object and initialization
	// ---------------------------------------------
	ss::Network network(ss::Endpoint::Controller, "Controller", CONTROLLER_PORT, endpoints, processVec->processes);

	network.initAsSamsonController();
	network.procVecSet(processVec, processVecSize, ss::platformProcessesSave);
	network.runInBackground();
	
	
	// Instance of the Samson Controller
	// ---------------------------------
	
	ss::SamsonController  controller(&network);

	controller.runBackgroundProcesses();
	controller.touch();
	
	while (true)
		sleep(10000);
}
