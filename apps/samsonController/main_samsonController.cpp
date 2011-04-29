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
#include "engine/MemoryManager.h"		// ss::MemoryManager
#include "engine/Engine.h"				// engine::Engine
#include "au/LockDebugger.h"    // au::LockDebugger

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
* global variables
*/
int                 logFd      = -1;
ss::ProcessVector*  processVec = NULL;
ss::Network*        networkP   = NULL;



/* ****************************************************************************
*
* exitFunction - 
*/
void exitFunction(void)
{
	LM_M(("Freeing stuff before EXITING"));
	LM_W(("process vector is sent to Network and freed by Network"));

	if (networkP)
	{
		LM_M(("deleting Network instance"));
		delete networkP;
		networkP = NULL;
	}

	if (progName)
	{
		LM_M(("Freeing progName"));
		free(progName);
		progName = NULL;
	}
}



/* ****************************************************************************
*
* main - main routine for the samsonController
*/
int main(int argC, const char* argV[])
{
	int  processVecSize;

	paConfig("prefix",                        (void*) "SSC_");
	paConfig("usage and exit on any warning", (void*) true);
	paConfig("log to screen",                 (void*) "only errors");
	paConfig("log file line format",          (void*) "TYPE:DATE:EXEC/FILE[LINE] FUNC: TEXT");
	paConfig("screen line format",            (void*) "TYPE@TIME  EXEC: TEXT");
	paConfig("log to file",                   (void*) true);

	paParse(paArgs, argC, (char**) argV, 1, false);

	LM_T(LmtInit, ("Started with arguments:"));
	for (int ix = 0; ix < argC; ix++)
		LM_T(LmtInit, ("  %02d: '%s'", ix, argV[ix]));

	atexit(exitFunction);
	atexit(google::protobuf::ShutdownProtobufLibrary);

	processVec = ss::platformProcessesGet(&processVecSize);
	if (processVec == NULL)
		LM_X(1, ("Error retreiving info about the platform processes - can't function without it!"));

	LM_T(LmtInit, ("%d workers", processVec->processes - 1));

	// Init singlentons
	au::LockDebugger::shared();         // Lock usage debugging (necessary here where there is only one thread)
	ss::SamsonSetup::load(workingDir);  // Load setup and create all directories
	engine::Engine::init();					// Init the SamsonEngine
	ss::ModulesManager::init();			// Init the modules manager
	// Goyo. Groping in the dark (blind sticks for an easier translation)
	engine::MemoryManager::init(  ss::SamsonSetup::shared()->memory );
	// Goyo. End of groping in the dark

	// Instance of network object and initialization
	// ---------------------------------------------
	networkP = new ss::Network(ss::Endpoint::Controller, "Controller", CONTROLLER_PORT, endpoints, processVec->processes - 1);

	networkP->initAsSamsonController();
	networkP->procVecSet(processVec, processVecSize, ss::platformProcessesSave);
	networkP->runInBackground();
	
	
	// Instance of the Samson Controller
	// ---------------------------------
	
	ss::SamsonController  controller(networkP);


	// Run the engine function
	engine::Engine::run();
	
}
