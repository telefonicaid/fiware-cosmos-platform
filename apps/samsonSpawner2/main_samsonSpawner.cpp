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

#include "parseArgs/parseArgs.h"             // parseArgs
#include "parseArgs/paConfig.h"              // paConfigCleanup
#include "logMsg/logMsg.h"                // LM_*
#include "logMsg/traceLevels.h"           // Trace levels

#include "samson/common/SamsonSetup.h"           // samson::SamsonSetup
#include "engine/MemoryManager.h"  // samson::MemoryManager
#include "samson/common/daemonize.h"             // daemonize
#include "SamsonSpawner.h"         // SamsonSpawner



/* ****************************************************************************
*
* Option variables
*/
bool  fg;
bool  noRestarts;
char  workingDir[1024];



#define DEF_WD  _i SAMSON_DEFAULT_WORKING_DIRECTORY
/* ****************************************************************************
*
* parse arguments
*/
PaArgument paArgs[] =
{
	{ "-fg",          &fg,          "FOREGROUND",   PaBool,    PaOpt,  false,    false,   true,  "don't start as daemon"   },
	{ "-noRestarts",  &noRestarts,  "NO_RESTARTS",  PaBool,    PaOpt,  false,    false,   true,  "don't restart processes" },
	{ "-working",      workingDir,  "WORKING",      PaString,  PaOpt,  DEF_WD,   PaNL,    PaNL,  "working directory"       },

	PA_END_OF_ARGS
};



/* ****************************************************************************
*
* Global variables
*/
int                     logFd             = -1;
samson::SamsonSpawner*      spawnerP          = NULL;



/* ****************************************************************************
*
* sigHandler - 
*/
void sigHandler(int sigNo)
{
	printf("Caught signal %d\n", sigNo);

	if (sigNo == SIGINT)
	{
		printf("Got SIGINT\n");
	}

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
	signal(SIGINT, sigHandler);

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

	if (fg == false)
		daemonize();

	atexit(exitFunction);
	
	samson::SamsonSetup::load(workingDir);
	engine::MemoryManager::init(samson::SamsonSetup::shared()->memory);

	spawnerP = new samson::SamsonSpawner();
	spawnerP->init();	
	spawnerP->run();

	return 0;
}
