
#include "Delilah.h"			// ss:Delilah
#include "SamsonWorker.h"		// ss::SamsonWorker
#include "SamsonController.h"	// ss:: SasonController
#include <vector>				// std::vector
#include <sstream>				// std::ostringstream
#include "traces.h"				// Traces stuff: samsonInitTrace(.) , ...
#include "Endpoint.h"			// ss::EndPoint

#include "FakeEndpoint.h"
#include "NetworkFake.h"
#include "NetworkFakeCenter.h"
#include "NetworkInterface.h"


#define VECTOR_LENGTH(v) sizeof(v)/sizeof(v[0])

const char* dalilah_argv_console[] = { "-controller" , "what_ever" ,"-console"};
const char* dalilah_argv[] = { "-controller" , "what_ever" };
const char* worker_argv[] = { "-controller" , "what_ever"};



int main(int argc, const char *argv[])
{
	// Init the trace system
	ss::samsonInitTrace( argc , argv );
	
	LM_T( TRACE_SAMSON_DEMO , ("Starting samsom demo") );
	
	au::CommandLine commandLine;
	commandLine.set_flag_int("workers", 2);			// Number of workers by command line ( default 2 )
	commandLine.set_flag_boolean("ncurses");

	// Command line to extract the number of workers from command line arguments
	commandLine.parse(argc , argv);
	int num_workers = commandLine.get_flag_int("workers");
	
	// Fake network element with N workers
	ss::NetworkFakeCenter center(num_workers);		
	
	// Create one controller, one dalilah and N workers
	ss::SamsonController controller ( argc, argv ,center.getNetwork( -1 )  );
	
	const char **_dalilah_argv;
	int _dalilah_argc;
	
	if( commandLine.get_flag_bool("ncurses") )
	{
		_dalilah_argv= dalilah_argv;
		_dalilah_argc = VECTOR_LENGTH(dalilah_argv);
	}
	else
	{
		_dalilah_argv= dalilah_argv_console;
		_dalilah_argc = VECTOR_LENGTH(dalilah_argv_console);
	}
	
	ss::Delilah dalilah( _dalilah_argc, _dalilah_argv , center.getNetwork( -2 )  );
	
	std::vector< ss::SamsonWorker* > workers;
	for (int i = 0 ; i < num_workers ; i ++ )
		workers.push_back( new ss::SamsonWorker( VECTOR_LENGTH(worker_argv) , worker_argv , center.getNetwork( i ) ) );
	
	controller.run();
	dalilah.run();
	for (int i = 0 ; i < num_workers ; i ++ )
		workers[i]->run();

	// Keep alive while dalila is alive ( sending packets in the background )
	center.run(  &dalilah.finish );
	
						  
}
