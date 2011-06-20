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

#include "parseArgs/parseArgs.h"          // parseArgs

#include "samson/common/ports.h"              // CONTROLLER_PORT
#include "samson/common/samsonDirectories.h"  // SAMSON_PLATFORM_PROCESSES
#include "samson/controller/SamsonController.h"	// samson::SamsonController
#include "samson/common/SamsonSetup.h"		// samson::SamsonSetup
#include "samson/common/platformProcesses.h"  // samson::platformProcessesGet, samson::platformProcessesSave
#include "engine/MemoryManager.h"		// samson::MemoryManager
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
samson::ProcessVector*  processVec = NULL;
samson::Network*        networkP   = NULL;



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

	processVec = samson::platformProcessesGet(&processVecSize);
	if (processVec == NULL)
		LM_X(1, ("Error retreiving info about the platform processes - can't function without it!"));

	LM_T(LmtInit, ("%d workers", processVec->processes - 1));

	// Init singlentons
	au::LockDebugger::shared();         // Lock usage debugging (necessary here where there is only one thread)
	samson::SamsonSetup::load(workingDir);  // Load setup and create all directories
	engine::Engine::init();					// Init the SamsonEngine
	samson::ModulesManager::init();			// Init the modules manager
	// Goyo. Groping in the dark (blind sticks for an easier translation)
	engine::MemoryManager::init(  samson::SamsonSetup::shared()->memory );
	// Goyo. End of groping in the dark

	// Instance of network object and initialization
	// ---------------------------------------------
	networkP = new samson::Network(samson::Endpoint::Controller, "Controller", CONTROLLER_PORT, endpoints, processVec->processes - 1);

	networkP->initAsSamsonController();
	networkP->procVecSet(processVec, processVecSize, samson::platformProcessesSave);
	networkP->runInBackground();
	
	
	// Instance of the Samson Controller
	// ---------------------------------
	
	samson::SamsonController  controller(networkP);


	// Run the engine function
	engine::Engine::run();
	
}
