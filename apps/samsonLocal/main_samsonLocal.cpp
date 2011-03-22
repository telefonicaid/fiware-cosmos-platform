#include "parseArgs.h"          // parseArgs

#include <vector>				// std::vector
#include <sstream>				// std::ostringstream

#include "logMsg.h"             // LM_*
#include "traceLevels.h"        // Trace Levels
#include "traces.h"				// Traces stuff: samsonInitTrace(.) , ...

#include "Endpoint.h"			// ss::EndPoint
#include "FakeEndpoint.h"
#include "NetworkFake.h"
#include "NetworkCenter.h"
#include "NetworkInterface.h"

#include "DelilahConsole.h"		// ss:DelilahConsole
#include "SamsonWorker.h"		// ss::SamsonWorker
#include "SamsonController.h"	// ss:: SasonController
#include "SamsonSetup.h"		// ss::SamsonSetup

#include "ModulesManager.h"		// ss::ModulesManager
#include "samson/Operation.h"	// ss::Operation
#include "SamsonSetup.h"		// ss::SamsonSetup

#include <string.h>				// strcmp
#include <signal.h>				// signal(.)

#include "Engine.h"				// ss::Engine

/*
 To be removed
 */

/**
 Examples to test the platform
 */

namespace ss {

	class EngineElementText : public EngineElement
	{
		std::string text;
	public:
		
		EngineElementText( std::string _text ) : EngineElement(  )
		{
			text = _text;
		}
		
		void run()
		{
			//std::cout << "Example: " << text << "\n";
		}
	};
	
	
	class EngineElementRepeatedText : public EngineElement
	{
		std::string text;
	public:
		
		EngineElementRepeatedText( std::string _text , int seconds ) : EngineElement( seconds )
		{
			text = _text;
		}
		
		void run()
		{
			//std::cout << "Example: " << text << "\n";
			//Engine::shared()->add( new EngineElementText( std::string("Callback: ") + text ) );
		}
	};

}


/* ****************************************************************************
 *
 * Option variables
 */
char             controller[80];
int              workers;
bool             noLog;
char			 workingDir[1024]; 	
char			 commandFileName[1024];


#define S01 (long int) "samson01:1234"
/* ****************************************************************************
 *
 * parse arguments
 */
PaArgument paArgs[] =
{
	{ "-controller",  controller,       "CONTROLLER",  PaString,  PaOpt,   S01,   PaNL,   PaNL,  "controller IP:port"  },
	{ "-workers",     &workers,         "WORKERS",     PaInt,     PaOpt,     1,      1,    100,  "number of workers"   },
	{ "-nolog",       &noLog,           "NO_LOG",      PaBool,    PaOpt, false,  false,   true,  "no logging"          },
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
	ss::DelilahConsole* delilahConsole = (ss::DelilahConsole*) d;

	delilahConsole->run();
	return NULL;
}

int main(int argC, const char *argV[])
{
	
	paConfig("prefix",                        (void*) "SSW_");
	paConfig("usage and exit on any warning", (void*) true);
	paConfig("log to screen",                 (void*) "only errors");
	paConfig("log file line format",          (void*) "TYPE:DATE:EXEC-AUX/FILE[LINE] FUNC: TEXT");
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
	
	ss::SamsonSetup::load( workingDir );		// Load setup and create default directories

	// Init singlelton in single thread mode
	ss::Engine::init();
	ss::ModulesManager::init();		// Init the modules manager
	
	// Google protocol buffer deallocation
	atexit(	google::protobuf::ShutdownProtobufLibrary );
	
	LM_M(("samsonLocal started with memory=%s and #processors=%d", au::Format::string( ss::SamsonSetup::shared()->memory, "B").c_str() , ss::SamsonSetup::shared()->num_processes ));
	
	
	// Fake network element with N workers
	ss::NetworkFakeCenter center(workers);		
	
	// Create one controller, one dalilah and N workers
	ss::SamsonController controller( center.getNetwork(-1) );
	
	ss::DelilahConsole delilahConsole( center.getNetwork(-2) );
	
	LM_T(LmtInit, ("SamsonLocal start"));
	LM_D(("Starting samson demo (logFd == %d)", ::logFd));

	std::vector< ss::SamsonWorker* > _workers;
	for (int i = 0 ; i < workers ; i ++ )
	{
		ss::SamsonWorker *w = new ss::SamsonWorker( center.getNetwork(i) );
		_workers.push_back(w);
	}

	
	// Run the network center in background
	center.runInBackground();


	// Run the samson engine
	ss::Engine::shared()->runInBackground();
	
	if ( strcmp( commandFileName,"") != 0 )
	{
		FILE *f = fopen( commandFileName , "r" );
		if( !f )
		{
			LM_E(("Error opening commands file %s", commandFileName));
			exit(0);
		}
		
		char line[1024];

		//LM_M(("Processing commands file %s", commandFileName ));
		while( fgets(line, sizeof(line), f) )
		{
			// Remove the last return of a string
			while( ( strlen( line ) > 0 ) && ( line[ strlen(line)-1] == '\n') > 0 )
				line[ strlen(line)-1]= '\0';
			
			//LM_M(("Processing line: %s", line ));
			size_t id = delilahConsole.runAsyncCommand( line );
			
			if( id != 0)
			{
				//LM_M(("Waiting until delilah-component %ul finish", id ));
				// Wait until this operation is finished
				while (delilahConsole.isActive( id ) )
					sleep(1);
			}
		}
		
		fclose(f);
		
		// Exit the engine
		ss::Engine::shared()->quit();
		
		// Destroy the engine
		ss::Engine::destroy();
		
		LM_M(("samsonLocal exit correctly"));
		
		exit(0);
	}
	
	
	
	// Run delilah client in foreground
	delilahConsole.run();	
	

	// Exit the engine
	ss::Engine::shared()->quit();
	
	// Destroy the engine
	ss::Engine::destroy();
	
	LM_M(("samsonLocal exit correctly"));
}






