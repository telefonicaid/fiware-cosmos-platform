/* ****************************************************************************
*
* FILE                     main_samsonController.cpp
*
* AUTHOR                   Ken Zangelin
*
* CREATION DATE            Dec 14 2010
*
*/
#include <unistd.h>                // read
#include <fcntl.h>                 // open, O_RDONLY, ...
#include <sys/stat.h>              // struct stat

#include "parseArgs/parseArgs.h"
#include "au/LockDebugger.h"
#include "engine/MemoryManager.h"
#include "engine/Engine.h"
#include "samson/common/platformProcesses.h"
#include "samson/common/SamsonSetup.h"
#include "samson/network/Network2.h"
#include "samson/network/Endpoint2.h"
#include "samson/network/EndpointManager.h"
#include "samson/controller/SamsonController.h"
#include "samson/common/samsonDirectories.h"



/* ****************************************************************************
*
* Option variables
*/
char workingDir[1024];
bool version;



#define DEF_WD   _i SAMSON_DEFAULT_WORKING_DIRECTORY
/* ****************************************************************************
*
* parse arguments
*/
PaArgument paArgs[] =
{
	{ "-working",  workingDir, "WORKING", PaString, PaOpt, DEF_WD,  PaNL,  PaNL, "working directory" },
	{ "--version", &version,   "VERSION", PaBool,   PaOpt, false,   false, true, "show version"      }, 

	PA_END_OF_ARGS
};



/* ****************************************************************************
*
* global variables
*/
int                   logFd    = -1;
samson::Network2*     networkP = NULL;



/* ****************************************************************************
*
* exitFunction - 
*/
void exitFunction(void)
{
	LM_W(("Freeing up stuff before EXITING"));

	if (networkP != NULL)
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
* helpText - 
*/
static const char* helpText = 
   "samsonController is the main process in the samson cluster,\n"
   "which consists of one or more workers and ONE controller ...\n";



/* ****************************************************************************
*
* main - main routine for the samsonController
*/
int main(int argC, const char* argV[])
{
	samson::EndpointManager* epMgr;

	paConfig("prefix",                        (void*) "SSC_");
	paConfig("usage and exit on any warning", (void*) true);
	paConfig("log to screen",                 (void*) "only errors");
	paConfig("log file line format",          (void*) "TYPE:DATE:EXEC/FILE[LINE] FUNC: TEXT");
	paConfig("screen line format",            (void*) "TYPE@TIME  EXEC: TEXT");
	paConfig("log to file",                   (void*) true);
	paConfig("help text",                     (void*) helpText);

	paParse(paArgs, argC, (char**) argV, 1, false);

	if (version)
	{
		printf("0.6\n");
		exit(1);
	}

	LM_T(LmtInit, ("Started with arguments:"));
	for (int ix = 0; ix < argC; ix++)
		LM_T(LmtInit, ("  %02d: '%s'", ix, argV[ix]));

	atexit(exitFunction);
	atexit(google::protobuf::ShutdownProtobufLibrary);

	// Init singletons
	au::LockDebugger::shared();             // Lock usage debugging (necessary here where there is only one thread)
	samson::SamsonSetup::load(workingDir);  // Load setup and create all directories
	engine::Engine::init();                 // Init the SamsonEngine
	samson::ModulesManager::init();         // Init the modules manager

	// Goyo. Groping in the dark (blind sticks for an easier translation)
	engine::MemoryManager::init(samson::SamsonSetup::shared()->memory);
	// Goyo. End of groping in the dark

	epMgr     = new samson::EndpointManager(samson::Endpoint2::Controller);
	networkP  = new samson::Network2(epMgr);

	networkP->runInBackground();
   	samson::SamsonController controller(networkP);

	// Run the engine function
    while( true )
        sleep(10);
}
