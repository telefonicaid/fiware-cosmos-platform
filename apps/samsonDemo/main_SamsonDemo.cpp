
#include "Delilah.h"			// ss:Delilah
#include "SamsonWorker.h"		// ss::SamsonWorker
#include "SamsonController.h"	// ss:: SasonController
#include <vector>				// std::vector
#include <sstream>				// std::ostringstream
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
	LmStatus  s;
	char*     trace = (char*) "0-255";
	
	progName = lmProgName((char*) argv[0], 1, false);
	
	if ((s = lmPathRegister("/tmp/", "DEF", "DEF", NULL)) != LmsOk)
		EXIT(1, ("lmPathRegister: %s", lmStrerror(s)));
	if ((s = lmFdRegister(1, "TYPE: TEXT", "DEF", "controller", NULL)) != LmsOk)
		EXIT(1, ("lmPathRegister: %s", lmStrerror(s)));
	if  ((s = lmInit()) != LmsOk)
		EXIT(1, ("lmInit: %s", lmStrerror(s)));
	
	if ((argc >= 3) && (strcmp(argv[1], "-t") == 0))
		trace = (char*) argv[2];
	if ((s = lmTraceSet(trace)) != LmsOk)
		EXIT(1, ("lmTraceSet: %s", lmStrerror(s)));
	
	LM_F(("set trace levels to '%s'", trace));
	for (int ix = 0; ix < 256; ix++)
		LM_T(ix,  ("Testing trace level %d", ix));
	
	
	au::CommandLine commandLine;
	commandLine.set_flag_int("workers", 2);			// Number of workers by command line ( default 2 )
	commandLine.set_flag_boolean("ncurses");

	commandLine.parse(argc , argv);
	int num_workers = commandLine.get_flag_int("workers");
	
	// Fake network element with N workers
	ss::NetworkFakeCenter center(num_workers);		

	
	// Creat the one controller, one dalilah and N workers
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
	
	while(!dalilah.finish)
		sleep(1);
	
						  
}
