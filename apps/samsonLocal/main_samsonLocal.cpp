#include "parseArgs/parseArgs.h"          // parseArgs

#include <vector>				// std::vector
#include <sstream>				// std::ostringstream
#include <string.h>                 // strcmp
#include <signal.h>                 // signal(.)

#include "au/LockDebugger.h"            // au::LockDebugger

#include "engine/Engine.h"                 // engine::Engine
#include "engine/EngineElement.h"          // engine::EngineElement
#include "engine/DiskManager.h"            // engine::DiskManager
#include "engine/ProcessManager.h"         // engine::ProcessManager

#include "logMsg/logMsg.h"             // LM_*
#include "logMsg/traceLevels.h"        // Trace Levels

#include "samson/common/traces.h"				// Traces stuff: samsonInitTrace(.) , ...
#include "samson/common/SamsonSetup.h"		// samson::SamsonSetup
#include "samson/common/SamsonSetup.h"		// samson::SamsonSetup

#include "samson/module/ModulesManager.h"		// samson::ModulesManager
#include "samson/module/Operation.h"	// samson::Operation


#include "samson/stream/BlockManager.h"     // samson::stream::BlockManager

#include "samson/isolated/ProcessItemIsolated.h"    // isolated_process_as_tread to put background process in thread mode
#include "samson/isolated/SharedMemoryManager.h"    // samson::SharedMemoryManager

#include "samson/network/Endpoint.h"			// samson::EndPoint
#include "samson/network/NetworkInterface.h"

#include "samson/worker/SamsonWorker.h"		// samson::SamsonWorker

#include "samson/controller/SamsonController.h"	// samson:: SasonController

#include "samson/delilah/DelilahConsole.h"              // ss:DelilahConsole
#include "samson/delilah/DelilahMonitorization.h"		// ss:DelilahMonitorization

#include "FakeEndpoint.h"
#include "NetworkFake.h"
#include "NetworkCenter.h"


/* ****************************************************************************
 *
 * Option variables
 */
char             controller[80];
int              workers;
bool             noLog;
char			 workingDir[1024]; 	
char			 commandFileName[1024];
bool             thread_mode;


#define S01 (long int) "samson01:1234"
/* ****************************************************************************
 *
 * parse arguments
 */
PaArgument paArgs[] =
{
	{ "-controller",  controller,       "CONTROLLER",  PaString,  PaOpt,   S01,   PaNL,   PaNL,  "controller IP:port"  },
	{ "-workers",     &workers,         "WORKERS",     PaInt,     PaOpt,     1,      1,    100,  "number of workers"   },
	{ "-nolog",       &noLog,           "NO_LOG",      PaBool,    PaOpt,    false,  false,   true,  "no logging"          },
	{ "-thread_mode", &thread_mode,     "THREAD_MODE", PaBool,    PaOpt,    false,  false,   true,  "thread_mode"          },
	{ "-working",     workingDir,       "WORKING",     PaString,  PaOpt,  _i SAMSON_DEFAULT_WORKING_DIRECTORY,   PaNL,   PaNL,  "Working directory"     },
	{ "-f",           commandFileName,  "FILE_NAME",   PaString,  PaOpt,  _i "",   PaNL,   PaNL,  "File with commands to run"     },
	PA_END_OF_ARGS
};

/* ****************************************************************************
 *
 * logFd - file descriptor for log file used in all libraries
 */
int logFd = -1;


void *run_DelilahConsole(void* d)
{
	samson::DelilahConsole* delilahConsole = (samson::DelilahConsole*) d;
	delilahConsole->run();

    exit(0);
    
	return NULL;
}

// Network center
samson::NetworkFakeCenter *center;

// Vector with the workers
std::vector< samson::SamsonWorker* > _workers;

void deleteWorkers()
{
    LM_M(("Removing workers"));
    for ( size_t i = 0 ; i < _workers.size() ; i++)
        delete _workers[i];
}


void deleteNetworkCenter()
{
    if ( center )
    {
        delete center;
        center = NULL;
    }
}

int main(int argC, const char *argV[])
{
        
	paConfig("prefix",                        (void*) "SSL_");
	paConfig("usage and exit on any warning", (void*) true);
	paConfig("log to screen",                 (void*) "only errors");
	paConfig("log file line format",          (void*) "TYPE:DATE:EXEC-AUX/FILE[LINE] (p.PID) FUNC: TEXT");
	paConfig("screen line format",            (void*) "TYPE: TEXT");
	paConfig("log to file",                   (void*) true);
	
	paParse(paArgs, argC, (char**) argV, 1, false);// No more pid in the log file name
	lmAux((char*) "father");
	logFd = lmFirstDiskFileDescriptor();
	
	/*
	LM_D(("Starting samson demo (logFd == %d)", ::logFd));
	for (int i = 0 ; i < 256 ; i++)
		LM_T(i,("Trace test %d",i));
	 */
    
    if( thread_mode )
    {
        LM_M(("samsonLocal: started in thread mode"));
        samson::ProcessItemIsolated::isolated_process_as_tread = true;
    }
    
    // Make sure this singlelton is created just once
    au::LockDebugger::shared();
    
    LM_M(("samsonLocal: Seting working directory as %s", workingDir ));
	samson::SamsonSetup::init(  );		// Load setup and create default directories
    samson::SamsonSetup::shared()->setWorkingDirectory(workingDir);

	engine::Engine::init();
    
    // Add this element to test how Engine crash for exesive task time
    // engine::Engine::add( new engine::EngineElementSleepTest() );
    
	samson::ModulesManager::init();		// Init the modules manager
	engine::SharedMemoryManager::init( samson::SamsonSetup::getInt("general.num_processess") , samson::SamsonSetup::getUInt64("general.shared_memory_size_per_buffer"));
	engine::DiskManager::init( 1 );
	engine::ProcessManager::init( samson::SamsonSetup::getInt("general.num_processess") );
	engine::MemoryManager::init(  samson::SamsonSetup::getUInt64("general.memory") );    
	
    // Block Manager
    samson::stream::BlockManager::init();
    
	// Google protocol buffer deallocation
	atexit(	google::protobuf::ShutdownProtobufLibrary );
	
	LM_M(("samsonLocal: Started with memory=%s and #processors=%d", au::str( samson::SamsonSetup::getUInt64("general.memory") , "B").c_str() , samson::SamsonSetup::getInt("general.num_processess") ));
	
	// Fake network element with N workers
    center = new samson::NetworkFakeCenter(workers);
    atexit(deleteNetworkCenter);
	
	// Create one controller, one dalilah and N workers
	samson::SamsonController controller( center->getNetwork(-1) );
	
	samson::DelilahConsole* delilahConsole = NULL;
    
    delilahConsole = new samson::DelilahConsole( center->getNetwork(-2) );
	
	LM_T(LmtInit, ("SamsonLocal start"));
	LM_D(("Starting samson demo (logFd == %d)", ::logFd));

	for (int i = 0 ; i < workers ; i ++ )
	{
		samson::SamsonWorker *w = new samson::SamsonWorker( center->getNetwork(i) );
		_workers.push_back(w);
	}
	
	// Run the network center in background
	center->runInBackground();

    // Set the command file name
    if( delilahConsole )
    {
        delilahConsole->setCommandfileName( commandFileName );
    
        // Run delilah console in background
        pthread_t t;
        pthread_create(&t, 0, run_DelilahConsole, delilahConsole);
    }
        
	// Not necessary anymore since engine starts automatically with the init call
	// engine::Engine::run();

    atexit(deleteWorkers);
    
    while( true )
        sleep(10);
   
}





