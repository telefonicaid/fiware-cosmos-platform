
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

#include "Delilah.h"			// ss:Delilah
#include "SamsonWorker.h"		// ss::SamsonWorker
#include "SamsonController.h"	// ss:: SasonController
#include "SamsonSetup.h"		// ss::SamsonSetup

#include "ModulesManager.h"		// ss::ModulesManager
#include "samson/Operation.h"	// ss::Operation
#include "SamsonSetup.h"		// ss::SamsonSetup

#define VECTOR_LENGTH(v) sizeof(v)/sizeof(v[0])

const char* dalilah_argv_basic[] = { "-controller" , "what_ever" ,"-basic"};
const char* dalilah_argv_console[] = { "-controller" , "what_ever" ,"-console"};
const char* dalilah_argv[] = { "-controller" , "what_ever" };

const char* worker_argv[] = { "-controller" , "what_ever","-alias","what_ever_alias","-no_log"};	//Necessary arguments at worker to avoid errors



/* ****************************************************************************
*
* logFd - file descriptor for log file used in all libraries
*/
int logFd = -1;

char* progName = (char*) "samsonDemo";

void *run_delilah(void* d)
{
	ss::Delilah* delilah = (ss::Delilah*) d;

	delilah->run();
	return NULL;
}



int main(int argc, const char *argv[])
{
	
	// Init the trace system
	ss::samsonInitTrace( argc , argv, &::logFd);
	
	LM_T(LMT_SAMSON_DEMO, ("Starting samson demo (logFd == %d)", ::logFd));
	
	au::CommandLine commandLine;
	commandLine.set_flag_boolean("console");
	commandLine.set_flag_boolean("basic");

	ss::SamsonSetup::shared();	// Load setup and create default directories
	
	// Command line to extract the number of workers from command line arguments
	commandLine.parse(argc , argv);
	
	int num_workers = ss::SamsonSetup::shared()->num_workers;
	assert( num_workers != -1 );
	
	// Fake network element with N workers
	ss::NetworkFakeCenter center(num_workers);		
	
	// Create one controller, one dalilah and N workers
	ss::SamsonController controller(center.getNetwork(-1), 1234, (char*) "/opt/samson/setup.txt", num_workers, 80);
	
	const char**  _dalilah_argv;
	int           _dalilah_argc;
	bool          console;
	bool          basic;
	
	if (commandLine.get_flag_bool("console"))
	{
		_dalilah_argv = dalilah_argv_console;
		_dalilah_argc = VECTOR_LENGTH(dalilah_argv_console);
		console = true;
	}
	else if (commandLine.get_flag_bool("basic"))
	{
		_dalilah_argv = dalilah_argv_basic;
		_dalilah_argc = VECTOR_LENGTH(dalilah_argv_basic);
		basic = true;
	}
	else
	{
		_dalilah_argv = dalilah_argv;
		_dalilah_argc = VECTOR_LENGTH(dalilah_argv);
	}
	
	ss::Delilah delilah(center.getNetwork(-2), _dalilah_argc, _dalilah_argv, "localhost:1234", num_workers, 80, console, basic);
	
	LM_T(LMT_SAMSON_DEMO, ("Starting samson demo (logFd == %d)", ::logFd));

	std::vector< ss::SamsonWorker* > workers;
	for (int i = 0 ; i < num_workers ; i ++ )
	{
		char alias[16];

		snprintf(alias, sizeof(alias), "worker%02d", i);
		ss::SamsonWorker *w = new ss::SamsonWorker((char*) "localhost:1234", alias, 1235 + i, 5, 80);

		w->networkSet( center.getNetwork(i));
		workers.push_back(w);
	}

	lmTraceSet((char*) "60");

	controller.run();
	for (int i = 0 ; i < num_workers ; i ++ )
		workers[i]->run();

	LM_T(LMT_SAMSON_DEMO, ("Starting samson demo (logFd == %d)", ::logFd));

	// Run client in another thread
	pthread_t t_delilah;
	pthread_create(&t_delilah, NULL, run_delilah, &delilah);
	
	LM_T(LMT_SAMSON_DEMO, ("Starting samson demo (logFd == %d)", ::logFd));
	// Keep alive while dalila is alive ( sending packets in the background )
	center.run(&delilah.finish);
}
