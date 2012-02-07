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
#include "samson/common/samsonVars.h"       // SAMSON_

#include "samson/module/ModulesManager.h"		// samson::ModulesManager
#include "samson/module/Operation.h"	// samson::Operation


#include "samson/stream/BlockManager.h"     // samson::stream::BlockManager

#include "samson/isolated/ProcessItemIsolated.h"    // isolated_process_as_tread to put background process in thread mode
#include "samson/isolated/SharedMemoryManager.h"    // samson::SharedMemoryManager

#include "samson/network/NetworkInterface.h"

#include "samson/worker/SamsonWorker.h"		// samson::SamsonWorker

#include "samson/delilah/DelilahConsole.h"              // ss:DelilahConsole

#include "FakeEndpoint.h"
#include "NetworkFake.h"
#include "NetworkCenter.h"

#include "samson/delilahQt/DelilahQt.h"


/* ****************************************************************************
 *
 * Option variables
 */
SAMSON_ARG_VARS;
int              workers;
bool             noLog;
char			 commandFileName[1024];
bool             thread_mode;
bool             delilah_qt;


#define S01 (long int) "samson01:1234"
/* ****************************************************************************
 *
 * parse arguments
 */
PaArgument paArgs[] =
{
	SAMSON_ARGS,

	{ "-workers",     &workers,         "WORKERS",     PaInt,     PaOpt,     1,      1,    100,  "number of workers"   },
	{ "-nolog",       &noLog,           "NO_LOG",      PaBool,    PaOpt,    false,  false,   true,  "no logging"          },
	{ "-thread_mode", &thread_mode,     "THREAD_MODE", PaBool,    PaOpt,    false,  false,   true,  "thread_mode"          },
	{ "-qt",          &delilah_qt,     "" ,            PaBool,    PaOpt,    false,  false,   true,  "Delilah Qt"          },
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
    // Free resources automatically when this thread finish
    pthread_detach(pthread_self());
    
	samson::DelilahConsole* delilahConsole = (samson::DelilahConsole*) d;
	delilahConsole->run();

    exit(0);
    
	return NULL;
}

// Network center
samson::NetworkFakeCenter *networkCenter;

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
    if ( networkCenter )
    {
        delete networkCenter;
        networkCenter = NULL;
    }
}

int main(int argC, const char *argV[])
{
	paConfig("usage and exit on any warning", (void*) true);
    paConfig("log to screen",                 (void*) false);
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
    
	samson::SamsonSetup::init( samsonHome , samsonWorking );		// Load setup and create default directories
    samson::SamsonSetup::shared()->createWorkingDirectories();      // Create working directories

	engine::Engine::init();
    
    // Add this element to test how Engine crash for exesive task time
    // engine::Engine::add( new engine::EngineElementSleepTest() );
    
	samson::ModulesManager::init();		// Init the modules manager
	engine::SharedMemoryManager::init( samson::SamsonSetup::shared()->getInt("general.num_processess") , samson::SamsonSetup::shared()->getUInt64("general.shared_memory_size_per_buffer"));
	engine::DiskManager::init( 1 );
	engine::ProcessManager::init( samson::SamsonSetup::shared()->getInt("general.num_processess") );
	engine::MemoryManager::init(  samson::SamsonSetup::shared()->getUInt64("general.memory") );    
	
    // Block Manager
    samson::stream::BlockManager::init();
    
	// Google protocol buffer deallocation
	atexit(	google::protobuf::ShutdownProtobufLibrary );
	
	LM_M(("samsonLocal: Started with memory=%s and #processors=%d", au::str( samson::SamsonSetup::shared()->getUInt64("general.memory") , "B").c_str() , samson::SamsonSetup::shared()->getInt("general.num_processess") ));
	
	// Fake network element with N workers
    networkCenter = new samson::NetworkFakeCenter(workers);
    atexit(deleteNetworkCenter);
	
    samson::DelilahConsole* delilahConsole = NULL;
    samson::DelilahQt* delilahQt = NULL;
    
    // Console delilah..
    if( delilah_qt )
        delilahQt = new samson::DelilahQt( networkCenter->getNetworkForDelilah() );
    else            
        delilahConsole = new samson::DelilahConsole( networkCenter->getNetworkForDelilah() );

	
	LM_V(("SamsonLocal start"));
	LM_D(("Starting samson demo (logFd == %d)", ::logFd));

	for (int i = 0 ; i < workers ; i ++ )
	{
		samson::SamsonWorker *w = new samson::SamsonWorker( networkCenter->getNetworkForWorker(i) );
		_workers.push_back(w);
	}
	
	// Run the network center in background
	networkCenter->runInBackground();
    
    // Set the command file name
    if( delilahConsole )
    {
        delilahConsole->setCommandfileName( commandFileName );
    
        // Run delilah console in background
        pthread_t t;
        pthread_create(&t, 0, run_DelilahConsole, delilahConsole);
    }

    // Set the command file name
    if( delilahQt )
        delilahQt->run();
    
	// Not necessary anymore since engine starts automatically with the init call
	// engine::Engine::run();

    atexit(deleteWorkers);
    
    while( true )
        sleep(10);
   
}






