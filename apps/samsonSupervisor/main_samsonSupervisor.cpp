/* ****************************************************************************
*
* FILE                     main_samsonSupervisor.cpp
*
* AUTHOR                   Ken Zangelin
*
* CREATION DATE            Dec 14 2010
*
*/
#include "logMsg.h"             // LM_*
#include "parseArgs.h"          // parseArgs

#include "processList.h"        // processListInit
#include "spawnerList.h"        // spawnerListInit
#include "starterList.h"        // starterListInit
#include "configFile.h"         // configFileParse

#include "Endpoint.h"           // Endpoint
#include "Network.h"            // Network
#include "TabManager.h"         // TabManager
#include "qt.h"                 // qtRun
#include "SamsonSupervisor.h"   // SamsonSupervisor



/* ****************************************************************************
*
* Global variables
*/
int                logFd        = -1;    // file descriptor for log file

ss::Network*       networkP     = NULL;
SamsonSupervisor*  supervisorP  = NULL;
ss::Endpoint*      controller   = NULL;
TabManager*        tabManager   = NULL;



/* ****************************************************************************
*
* Option variables
*/
int     endpoints;
char    controllerName[80];
char    cfPath[80];
bool    qt;



#define CFP (long int)  "/opt/samson/etc/platformProcesses"
#define NOC  (long int) "no controller"
/* ****************************************************************************
*
* Parse arguments
*/
PaArgument paArgs[] =
{
	{ "-controller",  controllerName,  "CONTROLLER",  PaString,  PaOpt,   NOC,  PaNL,   PaNL,  "controller IP:port"  },
	{ "-endpoints",   &endpoints,      "ENDPOINTS",   PaInt,     PaOpt,    80,     3,    100,  "number of endpoints" },
	{ "-config",      &cfPath,         "CF_FILE",     PaStr,     PaOpt,   CFP,  PaNL,   PaNL,  "path to config file" },
	{ "-qt",          &qt,             "QT",          PaBool,    PaOpt,  true, false,   true,  "graphical"           },

	PA_END_OF_ARGS
};



/* ****************************************************************************
*
* networkStart - 
*/
void* networkStart(void* vP)
{
	SamsonSupervisor* s = (SamsonSupervisor*) vP;

	networkP->setDataReceiver(s);
	networkP->setEndpointUpdateReceiver(s);
	networkP->setReadyReceiver(s);

	networkP->init(ss::Endpoint::Supervisor, "Supervisor", 0, controllerName);
	networkP->run();

	return NULL;
}



/* ****************************************************************************
*
* main - 
*/
int main(int argC, const char *argV[])
{
	pthread_t t;

	paConfig("prefix",                        (void*) "SSS_");
	paConfig("usage and exit on any warning", (void*) true);
	paConfig("log to screen",                 (void*) "only errors");
	paConfig("log file line format",          (void*) "TYPE:DATE:EXEC-AUX/FILE[LINE] FUNC: TEXT");
	paConfig("screen line format",            (void*) "TYPE: TEXT");
	paConfig("log to file",                   (void*) true);

	paParse(paArgs, argC, (char**) argV, 1, false);

	processListInit(20);
	spawnerListInit(10);
	starterListInit(30);

	configFileParse(cfPath);

	networkP    = new ss::Network(endpoints, 10);  // 10 workers by default
	supervisorP = new SamsonSupervisor(networkP);
	
	pthread_create(&t, NULL, networkStart, supervisorP);

	while (supervisorP->networkReady == false)
		usleep(10000);
	qtRun(argC, argV);

	return 0;
}
