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

#include <signal.h>


#include "au/LockDebugger.h"            // au::LockDebugger

#include "engine/MemoryManager.h"
#include "engine/Engine.h"
#include "engine/DiskManager.h"
#include "engine/ProcessManager.h"

#include "samson/common/samsonVersion.h"
#include "samson/common/samsonVars.h"
#include "samson/common/SamsonSetup.h"
#include "samson/network/WorkerNetwork.h"
#include "samson/worker/SamsonWorker.h"
#include "samson/isolated/SharedMemoryManager.h"
#include "samson/stream/BlockManager.h"
#include "samson/module/ModulesManager.h"



/* ****************************************************************************
*
* Option variables
*/
SAMSON_ARG_VARS;

bool     noLog;
int      port;
int      web_port;


/* ****************************************************************************
*
* parse arguments
*/
PaArgument paArgs[] =
{
	SAMSON_ARGS,
	{ "-port",      &port,      "",  PaInt,  PaOpt, SAMSON_WORKER_PORT,     1,  9999, "Port to receive new connections"   },
	{ "-web_port",  &web_port,  "",  PaInt,  PaOpt, SAMSON_WORKER_WEB_PORT, 1,  9999, "Port to receive new connections"   },
	{ "-nolog",     &noLog, "SS_WORKER_NO_LOG", PaBool, PaOpt, false,  false,   true,        "no logging"                        },
	PA_END_OF_ARGS
};



/* ****************************************************************************
*
* global variables
*/
int                   logFd             = -1;
samson::SamsonWorker* worker            = NULL;



/* ****************************************************************************
*
* exitFunction - 
*/
void exitFunction(void)
{
	if (worker)
		delete worker;

    google::protobuf::ShutdownProtobufLibrary();
    
}

/* ****************************************************************************
*
* man texts -
*/
static const char* manSynopsis         = " [OPTION]";
static const char* manShortDescription = "samsonWorker is the main process in a SAMSON system.\n\n";
static const char* manDescription      =
    "\n"
    "samsonWorker is the main process in a SAMSON system. All the nodes in the cluster has its own samsonWorker process\n"
    "samosonWorker is connected to a central process called samsonController"
    "All clients of the platform ( delila's ) are connected to all samsonWorkers in the system"
    "See samson documentation to get more information about how to get a SAMSON system up and running"
    "\n";

static const char* manExitStatus    = "0      if OK\n 1-255  error\n";
static const char* manAuthor        = "Written by Andreu Urruela, Ken Zangelin and J.Gregorio Escalada.";
static const char* manReportingBugs = "bugs to samson-dev@tid.es\n";
static const char* manCopyright     = "Copyright (C) 2011 Telefonica Investigacion y Desarrollo";
static const char* manVersion       = SAMSON_VERSION;


void captureSIGINT( int s )
{
    LM_X(1, ("Signal SIGINT"));
}

void captureSIGPIPE( int s )
{
    LM_M(("Capturing SIGPIPE"));
}


/* ****************************************************************************
*
* main - 
*/
int main(int argC, const char *argV[])
{
	paConfig("builtin prefix",                (void*) "SS_WORKER_");
	paConfig("usage and exit on any warning", (void*) true);
	paConfig("log to screen",                 (void*) "only errors");
	paConfig("log file line format",          (void*) "TYPE:DATE:EXEC-AUX/FILE[LINE](p.PID)(t.TID) FUNC: TEXT");
	paConfig("screen line format",            (void*) "TYPE@TIME  EXEC: TEXT");
	paConfig("log to file",                   (void*) true);

	paConfig("man synopsis",                  (void*) manSynopsis);
	paConfig("man shortdescription",          (void*) manShortDescription);
	paConfig("man description",               (void*) manDescription);
	paConfig("man exitstatus",                (void*) manExitStatus);
	paConfig("man author",                    (void*) manAuthor);
	paConfig("man reportingbugs",             (void*) manReportingBugs);
	paConfig("man copyright",                 (void*) manCopyright);
	paConfig("man version",                   (void*) manVersion);

    
	paParse(paArgs, argC, (char**) argV, 1, false);

	lmAux((char*) "father");

	LM_V(("Started with arguments:"));
	for (int ix = 0; ix < argC; ix++)
		LM_V(("  %02d: '%s'", ix, argV[ix]));

	logFd = lmFirstDiskFileDescriptor();
    
    // Capturing SIGPIPE
    if( signal( SIGPIPE , captureSIGPIPE ) == SIG_ERR )
        LM_W(("SIGPIPE cannot be handled"));

    if( signal( SIGINT , captureSIGINT ) == SIG_ERR )
        LM_W(("SIGINT cannot be handled"));
    
    // Make sure this singlelton is created just once
    au::LockDebugger::shared();
    
	samson::SamsonSetup::init(samsonHome , samsonWorking );          // Load setup and create default directories
    samson::SamsonSetup::shared()->createWorkingDirectories();      // Create working directories
    
	engine::Engine::init();
	engine::SharedMemoryManager::init(samson::SamsonSetup::shared()->getInt("general.num_processess") , samson::SamsonSetup::shared()->getUInt64("general.shared_memory_size_per_buffer"));
    
	engine::DiskManager::init(1);
	engine::ProcessManager::init(samson::SamsonSetup::shared()->getInt("general.num_processess"));
	engine::MemoryManager::init(samson::SamsonSetup::shared()->getUInt64("general.memory"));
	samson::ModulesManager::init();
    samson::stream::BlockManager::init();

    
	// Instance of network object and initialization
	// --------------------------------------------------------------------
	samson::WorkerNetwork*  networkP  = new samson::WorkerNetwork( port , web_port );
	
	// Instance of SamsonWorker object (network contains at least the number of wokers)
	// -----------------------------------------------------------------------------------
	worker = new samson::SamsonWorker(networkP);


    // Clean up function ( here to make sure, this cleanup function is called before Engine clean up function )
	atexit(exitFunction);
    
    LM_M(("Running"));
    
    while( true )
        sleep(10);
}
