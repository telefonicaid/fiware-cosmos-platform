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
#include "iomConnect.h"         // iomConnect
#include "ports.h"              // LOG_SERVER_PORT
#include "Endpoint.h"           // Endpoint
#include "Network.h"            // Network

#include "globals.h"            // tabManager, ...
#include "Popup.h"              // Popup
#include "processList.h"        // processListInit
#include "starterList.h"        // starterListInit
#include "configFile.h"         // configFileParse

#include "TabManager.h"         // TabManager
#include "SamsonSupervisor.h"   // SamsonSupervisor
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

	configFileInit("/opt/samson/etc/platformProcesses");
	processListInit(20);
	starterListInit(30);

	LM_M(("calling ss::Network with %d endpoints", endpoints));
	networkP    = new ss::Network(ss::Endpoint::Supervisor, "Supervisor", 0, endpoints);
	supervisorP = new SamsonSupervisor(networkP);

	networkP->setDataReceiver(supervisorP);
	networkP->setEndpointUpdateReceiver(supervisorP);
	networkP->setReadyReceiver(supervisorP);

	networkP->logServerSet("localhost");  // log server will always run in 'localhost' for samsonSupervisor ...


	//
	// 1. Connect to controller
	//
	ss::Endpoint* controller;
	Process*      controllerProcess;

	controller = networkP->controllerConnect(controllerName);

	int    args = 20;
	char*  argVec[20];

	LM_TODO(("If connected OK, ask the controller about its command line options - and fill LogConfig Window accordingly"));
	memset(argVec, 0, sizeof(argVec));
	if (configFileParse(controller->ip.c_str(), "Controller", &args, argVec) == -1)
		LM_W(("configFileParse failed for 'Controller@%s", controller->ip.c_str()));

	LM_TODO(("Here I should take command line option data from the 'platformProcesses' file"));
	LM_TODO(("Just make sure the IP of the Controller matches the -controller option"));
	controllerProcess = processAdd("Controller", controller->ip.c_str(), controller->port, controller, argVec, args);
	


	//
	// 2. Connect to controllers Spawner
	//
	int fd;
	fd = iomConnect(controller->ip.c_str(), SPAWNER_PORT);

	if (fd == -1)
	{
		char title[128];
		char message[256];

		snprintf(title, sizeof(title), "Error connecting to Controller's Spawner");
		snprintf(message, sizeof(message), "Can't connect to controller's Spawner at %s, port %d\nPlease start all spawners before running this application", controller->ip.c_str(), SPAWNER_PORT);
		new Popup(title, message);
		app.exec();
		exit(2);
	}

	//
	// Any pending error from controller conncetion ?
	// 
	if (controller->state != ss::Endpoint::Connected)
	{
	   char title[128];
	   char message[256];
	   
	   snprintf(title, sizeof(title), "Error connecting to Controller");
	   snprintf(message, sizeof(message), "Can't connect to controller at %s, port %d", controller->ip.c_str(), controller->port);
	   new Popup(title, message);
	}

	controllerProcess->spawnInfo->spawnerP = spawnerAdd("controllerSpawner", controller->ip.c_str(), SPAWNER_PORT);


	networkP->init();

	LM_T(LmtInit, ("calling runUntilReady"));
	networkP->runUntilReady();
	LM_T(LmtInit, ("runUntilReady done"));

	mainWinCreate(qApp);
	tabManager = new TabManager(mainWindow);

	mainWindow->show();

	LM_T(LmtInit, ("letting control to QT main loop"));
	qApp->exec();

	return 0;
}
