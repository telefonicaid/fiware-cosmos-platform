/* ****************************************************************************
*
* FILE            main_Delilah.cpp
*
* AUTHOR          Andreu Urruela
*
* DATE            July 2011
*
* DESCRIPTION
*
* Main file for the "delilah" console app
*
*/
#include "parseArgs/parseArgs.h"

#include "au/string.h"
#include "au/LockDebugger.h"            // au::LockDebugger
#include "au/ThreadManager.h"


#include "engine/MemoryManager.h"
#include "engine/Engine.h"
#include "engine/DiskManager.h"
#include "engine/ProcessManager.h"

#include "samson/module/ModulesManager.h"       // samson::ModulesManager

#include "samson/common/ports.h"
#include "samson/common/status.h"
#include "samson/common/samsonVersion.h"
#include "samson/common/SamsonSetup.h"

#include "samson/network/DelilahNetwork.h"
#include "samson/network/Packet.h"
#include "samson/network/HostMgr.h"
#include "samson/network/Host.h"

#include "samson/delilah/DelilahConsole.h"

#include "samson/common/status.h"

/* ****************************************************************************
*
* Option variables
*/
char             target_host[80];
char             user[1024];
char             password[1024];
int              target_port;

int				 memory_gb;
int				 load_buffer_size_mb;
char			 commandFileName[1024];
bool             monitorization;
char             command[1024]; 


#define LOC "localhost"
/* ****************************************************************************
*
* parse arguments
*/
PaArgument paArgs[] =
{
	{ "",                  target_host,           "",       PaString, PaOpt, _i LOC, PaNL, PaNL, "SAMSON server hostname "         },
	{ "",                 &target_port,           "",       PaInt, PaOpt, SAMSON_WORKER_PORT, 1, 99999, "SAMSON server port"         },
	{ "-user",             user,                  "",       PaString, PaOpt,  _i "anonymous", PaNL, PaNL, "User to connect to SAMSON cluster"  },
	{ "-password",         password,              "",       PaString, PaOpt,  _i "anonymous", PaNL, PaNL, "Password to connect to SAMSON cluster"  },
	{ "-memory",           &memory_gb,            "MEMORY",           PaInt,    PaOpt,      1,    1,  100, "memory in GBytes"           },
	{ "-load_buffer_size", &load_buffer_size_mb,  "LOAD_BUFFER_SIZE", PaInt,    PaOpt,     64,   64, 2048, "load buffer size in MBytes" },
	{ "-f",                 commandFileName,      "FILE_NAME",        PaString, PaOpt,  _i "", PaNL, PaNL, "File with commands to run"  },
	{ "-command",           command,              "MONITORIZATION_COMMAND", PaString, PaOpt,  _i "", PaNL, PaNL, "Single command to be executed"  },

	PA_END_OF_ARGS
};

/* ****************************************************************************
*
* logFd - file descriptor for log file used in all libraries
*/
int logFd = -1;



/* ****************************************************************************
*
* man texts -
*/
static const char* manSynopsis         = " [OPTION]";
static const char* manShortDescription =  "delilah is the command-line client for SAMSON system\n";
static const char* manDescription      =
    "\n"
    "delilah is the command-line client to upload & download data, run processing commands and monitor a SAMSON system.\n"
    "See pdf document about samson system to get more information about how to use delilah client"
    "\n";

static const char* manExitStatus    = "0      if OK\n 1-255  error\n";
static const char* manAuthor        = "Written by Andreu Urruela, Ken Zangelin and J.Gregorio Escalada.";
static const char* manReportingBugs = "bugs to samson-dev@tid.es\n";
static const char* manCopyright     = "Copyright (C) 2011 Telefonica Investigacion y Desarrollo";
static const char* manVersion       = SAMSON_VERSION;


// Custom name for the log file
extern char * paProgName;
size_t delilah_random_code;

/* ****************************************************************************
*
* main - 
*/
int main(int argC, const char *argV[])
{
	paConfig("prefix",                        (void*) "SSW_");
	paConfig("usage and exit on any warning", (void*) true);
	paConfig("log to screen",                 (void*) true);
	paConfig("log file line format",          (void*) "TYPE:DATE:EXEC-AUX/FILE[LINE](p.PID)(t.TID) FUNC: TEXT");
	paConfig("screen line format",            (void*) "TYPE: TEXT");
	paConfig("log to file",                   (void*) true);
    paConfig("log to stderr",         (void*) true);

    paConfig("man synopsis",                  (void*) manSynopsis);
    paConfig("man shortdescription",          (void*) manShortDescription);
    paConfig("man description",               (void*) manDescription);
    paConfig("man exitstatus",                (void*) manExitStatus);
    paConfig("man author",                    (void*) manAuthor);
    paConfig("man reportingbugs",             (void*) manReportingBugs);
    paConfig("man copyright",                 (void*) manCopyright);
    paConfig("man version",                   (void*) manVersion);

    // Random initialization
    struct timeval tp;
    gettimeofday(&tp, NULL);
    int rand_seq = tp.tv_sec*1000 + tp.tv_usec%1000;
    srand( rand_seq );

    // Random code for delilah
    delilah_random_code = au::code64_rand();
	paProgName = strdup( au::str("delilah_%s" , au::code64_str( delilah_random_code ).c_str() ).c_str() );
    
	paParse(paArgs, argC, (char**) argV, 1, true);
	lmAux((char*) "father");
	logFd = lmFirstDiskFileDescriptor();

    if ((strcmp(target_host, "localhost") == 0) || (strcmp(target_host, "127.0.0.1") == 0))
    {
        if (gethostname(target_host, sizeof(target_host)) != 0)
            LM_X(1, ("gethostname: %s", strerror(errno)));
        //LM_M(("Translated 'localhost' to %s", target_host));
    }


    // Make sure this singleton is created just once
    au::LockDebugger::shared();

    // Init samson setup with default values
	samson::SamsonSetup::init("","");			
	
	// Setup parameters from command line 
    size_t _memory = (size_t) memory_gb * (size_t) (1024*1024*1024);
    size_t _load_buffer_size = (size_t) load_buffer_size_mb * (size_t) (1024*1024);
	samson::SamsonSetup::shared()->setValueForParameter("general.memory", au::str("%lu",_memory));
    samson::SamsonSetup::shared()->setValueForParameter("load.buffer_size",  au::str("%lu",_load_buffer_size) );

    // Engine and its associated elements
	engine::Engine::init();
	engine::MemoryManager::init(samson::SamsonSetup::shared()->getUInt64("general.memory"));
	engine::DiskManager::init(1);
	engine::ProcessManager::init(samson::SamsonSetup::shared()->getInt("general.num_processess"));

	samson::ModulesManager::init();         // Init the modules manager
	
	// Initialize the network element for delilah
	samson::DelilahNetwork * networkP  = new samson::DelilahNetwork( "console" , delilah_random_code );
    
	// Create a DelilahControler once network is ready
	samson::DelilahConsole* delilahConsole = new samson::DelilahConsole( networkP );

    // Add main delilah connection with specified worker
    samson::Status s = networkP->addMainDelilahConnection( target_host , target_port , user , password );        
    
    if( s != samson::OK )
        LM_X(1, ("Not possible to open connection with %s:%d (%s)" , target_host , target_port , samson::status(s) ));
    
    // Only wait if there is a command or file
    if ( ( strcmp( command , "" ) != 0 ) && ( strcmp( commandFileName,"") != 0 ) )
    {
        std::cerr << "\nConnecting to SAMSOM cluster at " << target_host << " ...";
        std::cerr.flush();
        
        //
        // Wait until the network is ready II
        //
        while ( ! networkP->ready() )
        {
            LM_M(("Awaiting fully connected...."));
            usleep(1000);
        }
        
        std::cout << " OK\n";
        LM_M(("Connected to all workers"));
        
    }	
    
    // ----------------------------------------------------------------
    // Special mode with one command line command
    // ----------------------------------------------------------------
    
    if ( strcmp( command , "" ) != 0 )
    {
        {
            au::Cronometer cronometer;
            while ( !networkP->ready() ) 
            {
                usleep(100000);
                if( cronometer.diffTime() > 1 )
                {
                    LM_M(("Waiting delilah to connect to all workers"));
                    cronometer.reset();
                }
            }
        }
        
        delilahConsole->setSimpleOutput();
        size_t id = delilahConsole->runAsyncCommand( command );

        if( id != 0)
        {
            // Wait until this operation is finished
            while (delilahConsole->isActive( id ) )
            {
                // Wait until command is finished
                usleep(1000);
            }
            
            
            if( delilahConsole->hasError( id ) )
            {
                LM_E(("Error running '%s' \n", command ));
                LM_E(("Error: %s",  delilahConsole->errorMessage( id ).c_str()));
            }
            
        }
        
        exit(0);
    }
    
    LM_M(("Delilah random code %s" , au::code64_str( delilah_random_code ).c_str() ));
    LM_M(("Running delilah console..."));
    lmFdUnregister(2); // no more traces to stdout
    
    // ----------------------------------------------------------------
    // Special mode for file-based commands
    // ----------------------------------------------------------------

	if ( strcmp( commandFileName,"") != 0 )
	{
        
        // Set timple output
        delilahConsole->setSimpleOutput();
        
        {
            au::Cronometer cronometer;
            while ( !networkP->ready() ) 
            {
                usleep(100000);
                if( cronometer.diffTime() > 1 )
                {
                    LM_M(("Waiting delilah to connect to all workers"));
                    cronometer.reset();
                }
            }
        }
        
		FILE *f = fopen( commandFileName , "r" );
		if( !f )
		{
			LM_E(("Error opening commands file %s", commandFileName));
			exit(0);
		}
		
        int num_line = 0;
		char line[1024];
        
		LM_M(("Processing commands file %s", commandFileName ));

		while( fgets(line, sizeof(line), f) )
		{
			// Remove the last return of a string
			while( ( strlen( line ) > 0 ) && ( line[ strlen(line)-1] == '\n') > 0 )
				line[ strlen(line)-1]= '\0';
			
			//LM_M(("Processing line: %s", line ));
            num_line++;
            
            if( ( line[0] != '#' ) && ( strlen(line) > 0) )
            {
                
                size_t id = delilahConsole->runAsyncCommand( line );

                LM_M(("Processing: '%s' [ id generated %lu ]", line , id));
                
                if( id != 0)
                {
                    //LM_M(("Waiting until delilah-component %ul finish", id ));
                    // Wait until this operation is finished
                    while (delilahConsole->isActive( id ) )
                        usleep(1000);
                    
                    if( delilahConsole->hasError( id ) )
                    {
                        LM_E(("Error running '%s' at line %d", line, num_line));
                        LM_E(("Error: %s",  delilahConsole->errorMessage( id ).c_str()));
                    }
                    
                }
            }
		}
		
		fclose(f);

        // Flush content of console
        delilahConsole->flush();
		LM_M(("delilah exit correctly"));
		exit(0);
	}

    // Run console
    delilahConsole->run();

    // ------------------------------------------------------------------------
    // Close everything
    // ------------------------------------------------------------------------
    
    samson::ModulesManager::destroy();
    
	engine::ProcessManager::destroy();
	engine::DiskManager::destroy();
	engine::MemoryManager::destroy();
    engine::Engine::destroy();
    
	samson::SamsonSetup::destroy();
    
    
    // Check background threads
    au::StringVector background_threads = au::ThreadManager::shared()->getThreadNames();
    if( background_threads.size() > 0 )
    {
        LM_W(("Still %lu background threads running (%s)" , background_threads.size() , background_threads.str().c_str() ));
        std::cerr << au::ThreadManager::shared()->str();
    }
    else
        LM_M(("Finish correctly with 0 background processes"));
    
}
