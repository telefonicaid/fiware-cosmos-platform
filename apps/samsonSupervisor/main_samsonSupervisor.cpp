/* ****************************************************************************
*
* FILE                     main_samsonSupervisor.cpp
*
* AUTHOR                   Ken Zangelin
*
* CREATION DATE            Dec 14 2010
*
*/
#include <semaphore.h>

#include <QApplication>
#include <QWidget>
#include <QLabel>
#include <QDesktopWidget>
#include <QListWidget>

#include "logMsg.h"             // LM_*
#include "parseArgs.h"          // parseArgs

#include "processList.h"        // processListInit
#include "spawnerList.h"        // spawnerListInit
#include "starterList.h"        // starterListInit
#include "configFile.h"         // configFileParse

#include "Endpoint.h"           // Endpoint
#include "Network.h"            // Network
#include "iomConnect.h"         // iomConnect
#include "iomMsgSend.h"         // iomMsgSend
#include "TabManager.h"         // TabManager
#include "qt.h"                 // qtRun
#include "SamsonSupervisor.h"   // SamsonSupervisor
#include "ports.h"              // LOG_SERVER_PORT



/* ****************************************************************************
*
* Global variables
*/
int                logFd             = -1;    // file descriptor for log file
int                logServerFd       = -1;    // socket to send Log Lines to LogServer

ss::Network*       networkP          = NULL;
SamsonSupervisor*  supervisorP       = NULL;
ss::Endpoint*      controller        = NULL;
TabManager*        tabManager        = NULL;
ss::Endpoint*      logServerEndpoint = NULL;



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


#if 0
/* ****************************************************************************
*
* logHookFunction - 
*/
void logHookFunction(char* text, char type, const char* file, int lineNo, const char* fName, int tLev, const char* stre)
{
	int                       s;
	ss::Message::LogLineData  logLine;
	ss::Endpoint*             ep = logServerEndpoint;

	if ((logServerFd == -1) || (ep == NULL))
		return;

	memset(&logLine, 0, sizeof(logLine));

	logLine.type   = type;
	logLine.lineNo = lineNo;
	logLine.tLev   = tLev;

	if (text  != NULL)  strncpy(logLine.text,  text,  sizeof(logLine.text)  - 1);
	if (file  != NULL)  strncpy(logLine.file,  file,  sizeof(logLine.file)  - 1);
	if (fName != NULL)  strncpy(logLine.fName, fName, sizeof(logLine.fName) - 1);
	if (stre  != NULL)  strncpy(logLine.stre,  stre,  sizeof(logLine.stre));

	if (ep)
		s = iomMsgSend(ep->wFd, ep->name.c_str(), networkP->me->name.c_str(), ss::Message::LogLine, ss::Message::Msg, &logLine, sizeof(logLine), NULL);
	else
		s = iomMsgSend(logServerFd, "log provider", "logServer", ss::Message::LogLine, ss::Message::Msg, &logLine, sizeof(logLine), NULL);

	if (s < 0)
		logServerFd = -1;
}
#endif



/* ****************************************************************************
*
* main - 
*/
int main(int argC, const char *argV[])
{
    QApplication app(argC, (char**) argV);

	paConfig("prefix",                        (void*) "SSS_");
	paConfig("usage and exit on any warning", (void*) true);
	paConfig("log to screen",                 (void*) "only errors");
	paConfig("log file line format",          (void*) "TYPE:DATE:EXEC-AUX/FILE[LINE] FUNC: TEXT");
	paConfig("screen line format",            (void*) "TYPE: TEXT");
	paConfig("log to file",                   (void*) true);

	paParse(paArgs, argC, (char**) argV, 1, false);

	LM_TODO(("Try to connect to logServer as early as possible"));

	processListInit(20);
	spawnerListInit(10);
	starterListInit(30);

	configFileParse(cfPath);

	networkP    = new ss::Network(endpoints, 10);  // 10 workers by default
	supervisorP = new SamsonSupervisor(networkP);

	networkP->setDataReceiver(supervisorP);
	networkP->setEndpointUpdateReceiver(supervisorP);
	networkP->setReadyReceiver(supervisorP);

	networkP->init(ss::Endpoint::Supervisor, "Supervisor", 0, controllerName);

	logServerFd = iomConnect("localhost", LOG_SERVER_PORT);
	if (logServerFd == -1)
		LM_W(("error connecting to log server"));

	LM_M(("calling runUntilReady"));
	networkP->runUntilReady();
	LM_M(("runUntilReady done"));

	qtRun();

	return 0;
}
