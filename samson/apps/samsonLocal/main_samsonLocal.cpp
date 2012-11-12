/*
 * Telefónica Digital - Product Development and Innovation
 *
 * THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND,
 * EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
 *
 * Copyright (c) Telefónica Investigación y Desarrollo S.A.U.
 * All rights reserved.
 */
#include "parseArgs/parseArgs.h"          // parseArgs

#include <vector>				// std::vector
#include <sstream>				// std::ostringstream
#include <string.h>                 // strcmp
#include <signal.h>                 // signal(.)

#include "au/mutex/LockDebugger.h"            // au::LockDebugger
#include "au/ThreadManager.h"

#include "engine/Engine.h"                 // engine::Engine
#include "engine/EngineElement.h"          // engine::EngineElement
#include "engine/DiskManager.h"            // engine::DiskManager
#include "engine/ProcessManager.h"         // engine::ProcessManager

#include "logMsg/logMsg.h"             // LM_*
#include "logMsg/traceLevels.h"        // Trace Levels

#include "samson/common/ports.h"
#include "samson/common/traces.h"				// Traces stuff: samsonInitTrace(.) , ...
#include "samson/common/SamsonSetup.h"		// samson::SamsonSetup
#include "samson/common/SamsonSetup.h"		// samson::SamsonSetup
#include "samson/common/MemoryCheck.h"
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

//#include "samson/delilahQt/DelilahQt.h"


/* ****************************************************************************
 *
 * Option variables
 */
SAMSON_ARG_VARS;
int              workers;
char			 commandFileName[1024];
bool             thread_mode;
int web_port;         // Port to receive connection to the REST interface
int port;             // Not used but decessary to compile with samsonWorker

//bool             delilah_qt;


/* ****************************************************************************
 *
 * parse arguments
 */
PaArgument paArgs[] =
{
	SAMSON_ARGS,

	{ "-workers",     &workers,         "WORKERS",     PaInt,     PaOpt,        1,      1,    100,  "number of workers"         },
	{ "-thread_mode", &thread_mode,     "THREAD_MODE", PaBool,    PaOpt,    false,  false,   true,  "thread_mode"               },
//	{ "-qt",          &delilah_qt,     "" ,            PaBool,    PaOpt,    false,  false,   true,  "Delilah Qt"                },
	{ "-f",           commandFileName,  "FILE_NAME",   PaString,  PaOpt,    _i "",   PaNL,   PaNL,  "File with commands to run" },
    { "-web_port",  &web_port,  "",                         PaInt,    PaOpt, SAMSON_WORKER_WEB_PORT, 1,      9999,  "Port to receive web connections"   },
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
    
	samson::SamsonSetup::init( samsonHome , samsonWorking );		// Load setup and create default directories
    samson::SamsonSetup::shared()->createWorkingDirectories();      // Create working directories

    // Check to see if the current memory configuration is ok or not
    if (samson::MemoryCheck() == false)
        LM_X(1,("Insufficient memory configured. Check samsonLocalLog for more information."));

	engine::Engine::init();
    
    // Add this element to test how Engine crash for exesive task time
    // engine::Engine::add( new engine::EngineElementSleepTest() );
    
	samson::ModulesManager::init("samsonLocal");		// Init the modules manager
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
	
    // Init delilah and assign the network interface
    samson::DelilahConsole* delilahConsole = new samson::DelilahConsole(  );
    delilahConsole->setNetwork(networkCenter->getNetworkForDelilah());

	
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
        au::ThreadManager::shared()->addThread( "samsonLocal::delilah" , &t, 0, run_DelilahConsole, delilahConsole );
    }

    // Set the command file name
/*    if( delilahQt )
        delilahQt->run();
  */  
	// Not necessary anymore since engine starts automatically with the init call
	// engine::Engine::run();
    atexit(deleteWorkers);
    
    while( true )
        sleep(10);
   
}






