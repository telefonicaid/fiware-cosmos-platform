
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



#define VECTOR_LENGTH(v) sizeof(v)/sizeof(v[0])

const char* dalilah_argv_basic[] = { "-controller" , "what_ever" ,"-basic"};
const char* dalilah_argv_console[] = { "-controller" , "what_ever" ,"-console"};
const char* dalilah_argv[] = { "-controller" , "what_ever" };

const char* worker_argv[] = { "-controller" , "what_ever"};

void *run_delilah(void* d)
{
	ss::Delilah* delilah = (ss::Delilah*)d;
	delilah->run();
	return NULL;
}

int main(int argc, const char *argv[])
{
	// Init the trace system
	ss::samsonInitTrace( argc , argv );
	
	LM_T(LMT_SAMSON_DEMO, ("Starting samson demo"));
	
	au::CommandLine commandLine;
	commandLine.set_flag_int("workers", 2);			// Number of workers by command line ( default 2 )
	commandLine.set_flag_boolean("console");
	commandLine.set_flag_boolean("basic");

	// Command line to extract the number of workers from command line arguments
	commandLine.parse(argc , argv);
	int num_workers = commandLine.get_flag_int("workers");
	
	// Fake network element with N workers
	ss::NetworkFakeCenter center(num_workers);		
	
	// Create one controller, one dalilah and N workers
	ss::SamsonController controller ( argc, argv ,center.getNetwork( -1 )  );
	
	const char **_dalilah_argv;
	int _dalilah_argc;
	
	if( commandLine.get_flag_bool("console") )
	{
		_dalilah_argv= dalilah_argv_console;
		_dalilah_argc = VECTOR_LENGTH(dalilah_argv_console);
	}
	else if( commandLine.get_flag_bool("basic") )
	{
		_dalilah_argv= dalilah_argv_basic;
		_dalilah_argc = VECTOR_LENGTH(dalilah_argv_basic);
	}
	else
	{
		_dalilah_argv= dalilah_argv;
		_dalilah_argc = VECTOR_LENGTH(dalilah_argv);
	}
	
	ss::Delilah delilah( _dalilah_argc, _dalilah_argv , center.getNetwork( -2 )  );
	
	std::vector< ss::SamsonWorker* > workers;
	for (int i = 0 ; i < num_workers ; i ++ )
	{
		ss::SamsonWorker *w = new ss::SamsonWorker( VECTOR_LENGTH(worker_argv) , worker_argv );
		w->networkSet( center.getNetwork( i ) );
		workers.push_back( w );
		
	}
	
	controller.run();
	for (int i = 0 ; i < num_workers ; i ++ )
		workers[i]->run();

	// Run client in another thread
	pthread_t t_delilah;
	pthread_create(&t_delilah, NULL, run_delilah, &delilah);
	
	// Keep alive while dalila is alive ( sending packets in the background )
	center.run(&delilah.finish);
}
