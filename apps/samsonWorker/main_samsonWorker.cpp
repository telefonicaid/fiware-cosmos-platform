/* ****************************************************************************
*
* FILE                     main_samsonWorker.cpp
*
* AUTHOR                   Ken Zangelin
*
* CREATION DATE            Dec 14 2010
*
*/
#include "parseArgs/parseArgs.h"
#include "logMsg/logMsg.h"
#include "logMsg/traceLevels.h"

#include "engine/MemoryManager.h"
#include "engine/Engine.h"
#include "engine/DiskManager.h"
#include "engine/ProcessManager.h"

#include "samson/network/Network2.h"
#include "samson/network/Endpoint2.h"
#include "samson/network/EndpointManager.h"
#include "samson/worker/SamsonWorker.h"
#include "samson/common/SamsonSetup.h"
#include "samson/isolated/SharedMemoryManager.h"
#include "samson/stream/BlockManager.h"
#include "samson/module/ModulesManager.h"



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
int                   logFd  = -1;
samson::SamsonWorker* worker = NULL;



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

	samson::SamsonSetup::load(workingDir);  // Load setup and create default directories
    
	engine::SharedMemoryManager::init(samson::SamsonSetup::shared()->num_processes , samson::SamsonSetup::shared()->shared_memory_size_per_buffer);
	engine::Engine::init();
	engine::DiskManager::init(1);
	engine::ProcessManager::init(samson::SamsonSetup::shared()->num_processes);
	engine::MemoryManager::init(samson::SamsonSetup::shared()->memory);
	samson::ModulesManager::init();
    samson::stream::BlockManager::init();

    
	// Instance of network object and initialization
	// --------------------------------------------------------------------
	samson::EndpointManager* epMgr     = new samson::EndpointManager(samson::Endpoint2::Worker);
	samson::Network2*        networkP  = new samson::Network2(epMgr);


	LM_T(LmtInit, ("Waiting for network connection ..."));
	networkP->runInBackground();

	while (!networkP->ready())
		sleep(1);

	LM_T(LmtInit, ("Network Ready"));
	
	// Instance of SamsonWorker object (network contains at least the number of wokers)
	// -----------------------------------------------------------------------------------
	
	worker = new samson::SamsonWorker(networkP);

	// Not necessary anymore. Engine automatically starts with "init" call
	// engine::Engine::run();
    
    while( true )
        sleep(10);
}
