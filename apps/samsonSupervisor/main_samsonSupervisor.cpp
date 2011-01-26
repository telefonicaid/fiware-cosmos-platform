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
#include <QObject>
#include <QApplication>
#include <QWidget>
#include <QDesktopWidget>
#include <QIcon>
#include <QPushButton>
#include <QTextEdit>
#include <QSize>
#include <QVBoxLayout>
#include <QListWidget>
#include <QLabel>

#include "logMsg.h"             // LM_*
#include "traceLevels.h"        // Trace Levels
#include "parseArgs.h"          // parseArgs

#include "globals.h"            // tabManager, ...
#include "processList.h"        // processListInit
#include "spawnerList.h"        // spawnerListInit
#include "starterList.h"        // starterListInit
#include "configFile.h"         // configFileParse

#include "Endpoint.h"           // Endpoint
#include "Network.h"            // Network
#include "TabManager.h"         // TabManager
#include "SamsonSupervisor.h"   // SamsonSupervisor
#include "ports.h"              // LOG_SERVER_PORT
#include "actions.h"            // connectToAllSpawners



/* ****************************************************************************
*
* Window geometry
*/
#define MAIN_WIN_WIDTH     400
#define MAIN_WIN_HEIGHT    600



/* ****************************************************************************
*
* Global variables
*/
int                logFd             = -1;    // file descriptor for log file
ss::Network*       networkP          = NULL;
SamsonSupervisor*  supervisorP       = NULL;
ss::Endpoint*      controller        = NULL;
TabManager*        tabManager        = NULL;
ss::Endpoint*      logServerEndpoint = NULL;
QWidget*           mainWindow        = NULL;
QDesktopWidget*    desktop           = NULL;



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
	{ "-controller",  controllerName,  "CONTROLLER",  PaString,  PaReq,   NOC,  PaNL,   PaNL,  "controller IP:port"  },
	{ "-endpoints",   &endpoints,      "ENDPOINTS",   PaInt,     PaOpt,    80,     3,    100,  "number of endpoints" },
	{ "-config",      &cfPath,         "CF_FILE",     PaStr,     PaOpt,   CFP,  PaNL,   PaNL,  "path to config file" },
	{ "-qt",          &qt,             "QT",          PaBool,    PaOpt,  true, false,   true,  "graphical"           },

	PA_END_OF_ARGS
};



/* ****************************************************************************
*
* mainWinCreate - 
*/
static void mainWinCreate(QApplication* app)
{
	int              screenWidth;
	int              screenHeight;
	int              x;
	int              y;

	desktop    = QApplication::desktop();
	mainWindow = new QWidget();

	// Window Geometry
	screenWidth  = desktop->width();
	screenHeight = desktop->height();

	x = (screenWidth  - MAIN_WIN_WIDTH)  / 2;
	y = (screenHeight - MAIN_WIN_HEIGHT) / 2;

	mainWindow->resize(MAIN_WIN_WIDTH, MAIN_WIN_HEIGHT);
	mainWindow->move(x, y);
	mainWindow->setWindowTitle("Samson Supervisor");
}



/* ****************************************************************************
*
* main - 
*/
int main(int argC, const char *argV[])
{
    QApplication  app(argC, (char**) argV);

	paConfig("prefix",                        (void*) "SSS_");
	paConfig("usage and exit on any warning", (void*) true);
	paConfig("log to screen",                 (void*) "only errors");
	paConfig("log file line format",          (void*) "TYPE:DATE:EXEC-AUX/FILE[LINE] FUNC: TEXT");
	paConfig("screen line format",            (void*) "TYPE:EXEC: TEXT");
	paConfig("log to file",                   (void*) true);

	paParse(paArgs, argC, (char**) argV, 1, false);

	LM_T(LmtInit, ("Started with arguments:"));
	for (int ix = 0; ix < argC; ix++)
		LM_T(LmtInit, ("  %02d: '%s'", ix, argV[ix]));

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
	networkP->logServerSet("localhost");  // log server will always run in 'localhost' for samsonSupervisor ...

	LM_T(LmtInit, ("calling runUntilReady"));
	networkP->runUntilReady();
	LM_T(LmtInit, ("runUntilReady done"));

	mainWinCreate(qApp);
	tabManager = new TabManager(mainWindow);

	LM_T(LmtInit, ("Connecting to all Spawners"));
	connectToAllSpawners();

	LM_T(LmtInit, ("Connecting to all Workers"));
	unsigned int                ix;
	std::vector<ss::Endpoint*>  epV;

	epV = networkP->samsonWorkerEndpoints();
	LM_T(LmtInit, ("Got %d Worker endpoints", epV.size()));
	for (ix = 0; ix < epV.size(); ix++)
	{
		ss::Endpoint* ep;

		ep = epV[ix];

		LM_T(LmtInit, ("%02d: %-20s %-20s   %s", ix, ep->name.c_str(), ep->ip.c_str(), ep->stateName()));
	}

	mainWindow->show();

	LM_T(LmtInit, ("letting control to QT main loop"));
	qApp->exec();
	return 0;
}
