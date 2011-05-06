/* ****************************************************************************
*
* FILE                     main_samsonWorker.cpp
*
* AUTHOR                   Ken Zangelin
*
* CREATION DATE            Dec 14 2010
*
*/
#include "parseArgs.h"             // parseArgs
#include "logMsg.h"                // LM_*
#include "traceLevels.h"           // Trace levels

#include "Network2.h"              // Network2
#include "Endpoint2.h"             // Endpoint2
#include "EndpointManager.h"       // EndpointManager

#include "SamsonWorker.h"          // ss::SamsonWorker
#include "SamsonSetup.h"           // ss::SamsonSetup
#include "engine/MemoryManager.h"  // ss::MemoryManager
#include "engine/Engine.h"         // ss::Engine
#include "SharedMemoryManager.h"   // ss::SharedMemoryManager

#include "engine/DiskManager.h"    // engine::DiskManager
#include "engine/ProcessManager.h" // engine::ProcessManager



/* ****************************************************************************
*
* Option variables
*/
bool             noLog;
bool             local;
char             workingDir[1024];



#define NOLS    _i "no log server"
#define DEF_WD  _i SAMSON_DEFAULT_WORKING_DIRECTORY
/* ****************************************************************************
*
* parse arguments
*/
PaArgument paArgs[] =
{
	{ "-nolog",      &noLog,       "NO_LOG",      PaBool,    PaOpt,  false,  false,   true,  "no logging"            },
	{ "-local",      &local,       "LOCAL",       PaBool,    PaOpt,  false,  false,   true,  "local execution"       },
	{ "-working",     workingDir,  "WORKING",     PaString,  PaOpt, DEF_WD,   PaNL,   PaNL,  "working directory"     },

	PA_END_OF_ARGS
};



/* ****************************************************************************
*
* logFd - file descriptor for log file used in all libraries
*/
int               logFd  = -1;
ss::SamsonWorker* worker = NULL;



/* ****************************************************************************
*
* exitFunction - 
*/
void exitFunction(void)
{
	if (worker)
		delete worker;

	if (progName)
		free(progName);
}



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
	paConfig("screen line format",            (void*) "TYPE@TIME  EXEC: TEXT");
	paConfig("log to file",                   (void*) true);

	paParse(paArgs, argC, (char**) argV, 1, false);

	lmAux((char*) "father");

	atexit(google::protobuf::ShutdownProtobufLibrary);
	atexit(exitFunction);

	LM_T(LmtInit, ("Started with arguments:"));
	for (int ix = 0; ix < argC; ix++)
		LM_T(LmtInit, ("  %02d: '%s'", ix, argV[ix]));

	logFd = lmFirstDiskFileDescriptor();

	ss::SamsonSetup::load(workingDir);  // Load setup and create default directories
    
	engine::SharedMemoryManager::init(ss::SamsonSetup::shared()->num_processes * 2, ss::SamsonSetup::shared()->shared_memory_size_per_buffer);
	engine::Engine::init();
	ss::ModulesManager::init();
	engine::DiskManager::init(1);
	engine::ProcessManager::init(ss::SamsonSetup::shared()->num_processes);
	engine::MemoryManager::init(ss::SamsonSetup::shared()->memory);

    
	// Instance of network object and initialization
	// --------------------------------------------------------------------
	ss::EndpointManager* epMgr     = new ss::EndpointManager(ss::Endpoint2::Worker);
	ss::Network2*        networkP  = new ss::Network2(epMgr);


	LM_T(LmtInit, ("Waiting for network connection ..."));
	networkP->runInBackground();

	while (!networkP->ready())
		sleep(1);

	LM_T(LmtInit, ("Network Ready"));
	
	// Instance of SamsonWorker object (network contains at least the number of wokers)
	// -----------------------------------------------------------------------------------
	
	worker = new ss::SamsonWorker(networkP);

	// Run the main engine
	engine::Engine::run();
}
