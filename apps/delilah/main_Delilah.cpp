#include "parseArgs.h"          // parseArgs
#include "DelilahConsole.h"		// ss::DelilahConsole
#include "SamsonSetup.h"		// ss::SamsonSetup
#include "au/Format.h"				// au::Format
#include "MemoryManager.h"      // ss::MemoryManager
#include "Engine.h"				// engine::Engine


/* ****************************************************************************
*
* Option variables
*/
int              endpoints;
int              workers;
char             controller[80];
int				 memory_gb;
int				 load_buffer_size_mb;
char			 commandFileName[1024];



#define LOC "localhost:1234"
/* ****************************************************************************
*
* parse arguments
*/
PaArgument paArgs[] =
{
	{ "-controller",		controller,				"CONTROLLER",  PaString,  PaOpt, _i LOC,   PaNL,   PaNL,  "controller IP:port"  },
	{ "-endpoints",			&endpoints,				"ENDPOINTS",   PaInt,     PaOpt,     80,      3,    100,  "number of endpoints" },
	{ "-workers",			&workers,				"WORKERS",     PaInt,     PaOpt,      1,      1,    100,  "number of workers"   },
	{ "-memory",			&memory_gb,				"MEMORY",     PaInt,     PaOpt,      1,      1,    100,  "memory in GBytes"   },
	{ "-load_buffer_size",  &load_buffer_size_mb,   "LOAD_BUFFER_SIZE",     PaInt,     PaOpt,      64,      64,    2048,  "load buffer size in Mbytes"   },
	{ "-f",           commandFileName,  "FILE_NAME",   PaString,  PaOpt,  _i "",   PaNL,   PaNL,  "File with commands to run"     },

	PA_END_OF_ARGS
};



/* ****************************************************************************
*
* logFd - file descriptor for log file used in all libraries
*/
int logFd = -1;



/* ****************************************************************************
*
* main - 
*/
int main(int argC, const char *argV[])
{
	paConfig("prefix",                        (void*) "SSW_");
	paConfig("usage and exit on any warning", (void*) true);
	paConfig("log to screen",                 (void*) true);
	paConfig("log file line format",          (void*) "TYPE:DATE:EXEC-AUX/FILE[LINE] FUNC: TEXT");
	paConfig("log to file",                   (void*) true);

	paParse(paArgs, argC, (char**) argV, 1, false);
	lmAux((char*) "father");
	logFd = lmFirstDiskFileDescriptor();
	
	ss::SamsonSetup::load();			// Load the main setup file
	
	// Setup parameters from command line ( this is delilah so memory and load buffer size are configurable from command line )
	ss::SamsonSetup::shared()->memory			= (size_t) memory_gb * (size_t) (1024*1024*1024);
	ss::SamsonSetup::shared()->load_buffer_size = (size_t) load_buffer_size_mb * (size_t) (1024*1024);

	engine::Engine::init();
	// Goyo. Groping in the dark (blind sticks for an easier translation)
	engine::MemoryManager::init(  ss::SamsonSetup::shared()->memory );
	// Goyo. End of groping in the dark

	
	std::cout << "Waiting for network connection ...";
	
	// Init the network element for delilah
	ss::Network  network(ss::Endpoint::Delilah, "delilah", 0, endpoints, workers);
	
	network.init(controller);
	network.runInBackground();

	// What until the network is ready
	while ( !network.ready() )
		sleep(1);
	std::cout << "OK\n";

	// Create a DelilahControler once network is ready
	ss::DelilahConsole delilahConsole( &network );
	
	
	engine::Engine::runInBackground();
	
    
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
		
        
		LM_M(("samsonLocal exit correctly"));
		
		exit(0);
	}    
    
	delilahConsole.run();
	
}
