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

#include "engine/MemoryManager.h"
#include "engine/Engine.h"

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
	
	samson::SamsonSetup::load(workingDir);
	engine::Engine::init();
	engine::MemoryManager::init(samson::SamsonSetup::shared()->memory);

	spawnerP = new samson::SamsonSpawner();
	spawnerP->init();	
	spawnerP->run();

	return 0;
}
