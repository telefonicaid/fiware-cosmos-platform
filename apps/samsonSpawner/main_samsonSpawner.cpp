/* ****************************************************************************
*
* FILE                     main_samsonSpawner.cpp
*
* AUTHOR                   Ken Zangelin
*
* CREATION DATE            Dec 14 2010
*
*/
#include <signal.h>                // kill, SIGINT, ...

#include "parseArgs/parseArgs.h"
#include "parseArgs/paConfig.h"
#include "logMsg/logMsg.h"
#include "logMsg/traceLevels.h"

#include "au/LockDebugger.h"            // au::LockDebugger

#include "engine/MemoryManager.h"
#include "engine/Engine.h"

#include "samson/common/samsonVersion.h"
#include "samson/common/SamsonSetup.h"
#include "samson/common/samsonDirectories.h"

#include "samson/common/daemonize.h"
#include "SamsonSpawner.h"



/* ****************************************************************************
*
* Option variables
*/
bool  fg;
bool  noRestarts;
bool  reset;
char  workingDir[1024];



#define DEF_WD  _i SAMSON_DEFAULT_WORKING_DIRECTORY
/* ****************************************************************************
*
* parse arguments
*/
PaArgument paArgs[] =
{
	{ "-fg",      &fg,          "FOREGROUND",   PaBool,    PaOpt,  false,    false,   true,  "don't start as daemon"   },
	{ "-nr",      &noRestarts,  "NO_RESTARTS",  PaBool,    PaOpt,  false,    false,   true,  "don't restart processes" },
	{ "-reset",   &reset,       "RESET",        PaBool,    PaOpt,  false,    false,   true,  "reset"                   },
	{ "-working",  workingDir,  "WORKING",      PaString,  PaOpt,  DEF_WD,   PaNL,    PaNL,  "working directory"       },

	PA_END_OF_ARGS
};



/* ****************************************************************************
*
* Global variables
*/
int                     logFd    = -1;
samson::SamsonSpawner*  spawnerP = NULL;



/* ****************************************************************************
*
* sigHandler - 
*/
void sigHandler(int sigNo)
{
	if (reset == true)
	{
	   //LM_W(("Not dying by this signal %d as I initiated it myself ...", sigNo));
		reset = false;
		return;
	}

	LM_W(("Caught signal %d\n", sigNo));
	exit(1);
}



/* ****************************************************************************
*
* exitFunction - 
*/
void exitFunction(void)
{
	if (spawnerP)
		delete spawnerP;

	if (progName)
		free(progName);
}



/* ****************************************************************************
*
* man texts -
*/
static const char* manSynopsis         = " [OPTION]";
static const char* manShortDescription = "samsonSpawner is the process in charge of starting other processes in the samson cluster\n\n"
   "Complete list of options:\n";
static const char* manDescription      = 
"The samsonSpawner process, as its name indicates, is in charge of starting\n"
"all samson processes in the samson cluster.\n"
"A samsonSpawner process must be running in a computer that is to be used as a samson node.\n"
"It is seen to that this process is started automatically, so a node prepared to work as\n"
"a samson node shouldn't have a problem with this, but in case 'samsonSetup' is reporting\n"
"problems connecting to a samson node, at setting up the cluster (or at resetting it),\n"
"the first thing to do is of course to see whether an IP connection exists, trying with 'ping',\n"
"and the next step would be to log into the node and use 'ps' to see whether the samsonSpawner\n"
"process is up and running:\n"
"\n"
"  % ping <node>\n"
"  % ssh samson@<node>\n"
"  % ps aux | grep samsonSpawner\n\n";

static const char* manExitStatus       = "0      if OK\n 1-255  error\n";
static const char* manAuthor           = "Written by Andreu Urruela, Ken Zangelin and J.Gregorio Escalada.";
static const char* manReportingBugs    = "bugs to samson-bug-report@tid.es\nSamson home page: <http://www.tid.es/products/samson>";
static const char* manCopyright        = "Copyright (C) 2011 Telefonica Investigacion y Desarrollo";
static const char* manVersion          = SAMSON_VERSION;



/* ****************************************************************************
*
* main - 
*/
int main(int argC, const char *argV[])
{
	signal(SIGINT,  sigHandler);
	signal(SIGTERM, sigHandler);

	paConfig("prefix",                        (void*) "SSS_");
	paConfig("usage and exit on any warning", (void*) true);
	paConfig("log to screen",                 (void*) "only errors");
	paConfig("log file line format",          (void*) "TYPE:DATE:EXEC/FILE[LINE] FUNC: TEXT");
	paConfig("screen line format",            (void*) "TYPE@TIME  EXEC: TEXT (FUNC)");
	paConfig("log to file",                   (void*) true);

    paConfig("man synopsis",                  (void*) manSynopsis);
    paConfig("man shortdescription",          (void*) manShortDescription);
    paConfig("man description",               (void*) manDescription);
    paConfig("man exitstatus",                (void*) manExitStatus);
    paConfig("man author",                    (void*) manAuthor);
    paConfig("man reportingbugs",             (void*) manReportingBugs);
    paConfig("man copyright",                 (void*) manCopyright);
    paConfig("man version",                   (void*) manVersion);

	paParse(paArgs, argC, (char**) argV, 1, false);
	paConfigCleanup();

	LM_T(LmtInit, ("Started with arguments:"));
	for (int ix = 0; ix < argC; ix++)
		LM_T(LmtInit, ("  %02d: '%s'", ix, argV[ix]));

	if (reset == true)
	{
		if (access(SAMSON_PLATFORM_PROCESSES, R_OK) == 0)
		{
			if (unlink(SAMSON_PLATFORM_PROCESSES) != 0)
				LM_X(1, ("Sorry, unable to remove '%s'", SAMSON_PLATFORM_PROCESSES));
		}

		int sys_res = system("killall samsonSpawner");
		if( sys_res != 0 )
		   LM_W(("system call with killall failed"));
		usleep(200000);
	}

	if (fg == false)
		daemonize();

	atexit(exitFunction);

    // Make sure this singlelton is created just once
    au::LockDebugger::shared();
	
    
	samson::SamsonSetup::init();
    samson::SamsonSetup::shared()->setWorkingDirectory(workingDir);
    
	engine::Engine::init();
	engine::MemoryManager::init( samson::SamsonSetup::getUInt64("general.memory") );

	spawnerP = new samson::SamsonSpawner();
	spawnerP->init();	
	spawnerP->run();

	return 0;
}
