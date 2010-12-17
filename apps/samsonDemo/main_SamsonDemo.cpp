
#include <vector>				// std::vector
#include <sstream>				// std::ostringstream

#include "logMsg.h"             // LM_*
#include "traceLevels.h"        // LMT_*
#include "traces.h"				// Traces stuff: samsonInitTrace(.) , ...

#include "Endpoint.h"			// ss::EndPoint
#include "FakeEndpoint.h"
#include "NetworkFake.h"
#include "NetworkFakeCenter.h"
#include "NetworkInterface.h"

#include "DelilahConsole.h"		// ss:DelilahConsole
#include "SamsonWorker.h"		// ss::SamsonWorker
#include "SamsonController.h"	// ss:: SasonController
#include "SamsonSetup.h"		// ss::SamsonSetup

#include "ModulesManager.h"		// ss::ModulesManager
#include "samson/Operation.h"	// ss::Operation
#include "SamsonSetup.h"		// ss::SamsonSetup


/* ****************************************************************************
*
* logFd - file descriptor for log file used in all libraries
*/
int logFd = -1;

char* progName = (char*) "samsonDemo";

void *run_DelilahConsole(void* d)
{
	ss::DelilahConsole* delilahConsole = (ss::DelilahConsole*) d;

	delilahConsole->run();
	return NULL;
}

template<class C>
void *run_in_background(void* d)
{
	C* c = (C*) d;
	c->run();
	return NULL;
}


int main(int argc, const char *argv[])
{
	
	// Init the trace system
	ss::samsonInitTrace( argc , argv, &::logFd);
	
	LM_T(LMT_SAMSON_DEMO, ("Starting samson demo (logFd == %d)", ::logFd));
	
	au::CommandLine commandLine;
	commandLine.set_flag_int("workers",2);
	commandLine.set_flag_boolean("basic");

	ss::SamsonSetup::shared();	// Load setup and create default directories
	
	// Command line to extract the number of workers from command line arguments
	commandLine.parse(argc , argv);
	
	int num_workers = commandLine.get_flag_int("workers");
	assert( num_workers != -1 );
	
	// Fake network element with N workers
	ss::NetworkFakeCenter center(num_workers);		
	
	// Create one controller, one dalilah and N workers
	ss::SamsonController controller( center.getNetwork(-1) );
	
	ss::Delilah delilah(center.getNetwork(-2));
	ss::DelilahConsole delilahConsole( &delilah , !commandLine.get_flag_bool("basic") );
	
	LM_T(LMT_SAMSON_DEMO, ("Starting samson demo (logFd == %d)", ::logFd));

	std::vector< ss::SamsonWorker* > workers;
	for (int i = 0 ; i < num_workers ; i ++ )
	{
		ss::SamsonWorker *w = new ss::SamsonWorker( center.getNetwork(i) );
		workers.push_back(w);
	}

	lmTraceSet((char*) "60");

	
	LM_T(LMT_SAMSON_DEMO, ("Starting samson demo (logFd == %d)", ::logFd));

	// Run the network center in background
	pthread_t t;
	pthread_create(&t, NULL, run_in_background<ss::NetworkFakeCenter> , &center);
	
	// Run delilah client in foreground
	delilahConsole.run();
	
	
	assert( false );	// We never come back to here
	
}
