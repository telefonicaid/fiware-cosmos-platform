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
#include "ConnectionMgr.h"      // ConnectionMgr
#include "QueueMgr.h"           // QueueMgr
#include "UserMgr.h"            // UserMgr
#include "SamsonSupervisor.h"   // SamsonSupervisor
#include "actions.h"            // connectToAllSpawners
#include "permissions.h"        // Permissions
#include "LoginWindow.h"        // LoginWindow



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
bool               qtAppRunning      = false;
ConnectionMgr*     connectionMgr     = NULL;
QueueMgr*          queueMgr          = NULL;
UserMgr*           userMgr           = NULL;



/* ****************************************************************************
*
* Option variables
*/
int     endpoints;
char    controllerHost[80];
char    cfPath[80];



#define CFP (long int)  "/opt/samson/etc/platformProcesses"
#define NOC  (long int) "no controller"
/* ****************************************************************************
*
* Parse arguments
*/
PaArgument paArgs[] =
{
	{ "-controller",  controllerHost,  "CONTROLLER",  PaString,  PaReq,   NOC,  PaNL,   PaNL,  "controller IP"       },
	{ "-endpoints",   &endpoints,      "ENDPOINTS",   PaInt,     PaOpt,    80,     3,    100,  "number of endpoints" },
	{ "-config",      &cfPath,         "CF_FILE",     PaStr,     PaOpt,   CFP,  PaNL,   PaNL,  "path to config file" },

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
* login - 
*/
void login(void)
{
	new LoginWindow();
}



/* ****************************************************************************
*
* main - 
*/
int main(int argC, const char *argV[])
{
    QApplication   app(argC, (char**) argV);
	ss::Endpoint*  controller;
	Process*       controllerProcess;

	Q_INIT_RESOURCE(samsonSupervisor);

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

	userMgr = new UserMgr(10);

	userMgr->insert("superman", "samsonite", UpAll);
	userMgr->insert("nadie",    "",          UpNothing);
	userMgr->insert("3rdParty", "please",    UpStartProcesses | UpStopProcesses);

	login();

	LM_TODO(("Try to connect to logServer as early as possible"));

	configFileInit("/opt/samson/etc/platformProcesses");
	processListInit(20);
	starterListInit(30);

	LM_T(LmtInit, ("calling ss::Network with %d endpoints", endpoints));
	networkP    = new ss::Network(ss::Endpoint::Supervisor, "Supervisor", 0, endpoints);
	supervisorP = new SamsonSupervisor(networkP);

	networkP->setDataReceiver(supervisorP);
	networkP->setEndpointUpdateReceiver(supervisorP);
	networkP->setReadyReceiver(supervisorP);

	LM_T(LmtInit, ("Setting logServer to localhost and trying to connect to it ..."));
	networkP->logServerSet("localhost");  // log server will always run in 'localhost' for samsonSupervisor ...

	

	//
	// 1. Connect to Spawner in controller host
	//
	int fd;
	LM_T(LmtInit, ("Trying to connect to controller in '%s'", controllerHost));
	fd = iomConnect(controllerHost, SPAWNER_PORT);

	if (fd == -1)
	{
		char title[128];
		char message[256];

		snprintf(title, sizeof(title), "Error connecting to Spawner process in '%s'", controllerHost);
		snprintf(message, sizeof(message), "Can't connect to controller's Spawner at %s, port %d\nPlease start all spawners before running this application", controllerHost, SPAWNER_PORT);
		new Popup(title, message, true);
		app.exec();
		exit(2);
	}



	//
	// 2. Try to connect to controller
	//
	LM_T(LmtInit, ("Connecting to controller in '%s'", controllerHost));
	controller = networkP->controllerConnect(controllerHost);

	int    args = 0;
	char*  argVec[20];

	memset(argVec, 0, sizeof(argVec));

	if (controller->state != ss::Endpoint::Connected)
	{
		char eText[256];

		LM_TODO(("Not connected to controller - ask the config file about its command line options - and fill LogConfig Window accordingly"));
		args = 20;
		if (configFileParse(controllerHost, "Controller", &args, argVec) == -1)
		{
			snprintf(eText,
					 sizeof(eText),
					 "Unable to connect to controller in '%s'.\nAlso unable to find info on Controller in config file.\nUnable to connect to Controller, sorry.",
					 controllerHost);
			new Popup("Cannot connect to Controller", eText, true);
		}
		else
		{
			LM_W(("configFileParse returned %d args for 'Controller'", args));
			snprintf(eText, sizeof(eText), "Can't connect to controller at %s, port %d", controller->ip.c_str(), controller->port);
			new Popup("Error connecting to Controller", eText, false);
		}
	}

	LM_TODO(("If connect OK, the config file isn't parsed and so, argVec will be NULL and args == 0 ..."));
	controllerProcess = processAdd("Controller", controller->ip.c_str(), controller->port, controller, argVec, args);
	controllerProcess->spawnInfo->spawnerP = spawnerAdd("Spawner", controller->ip.c_str(), SPAWNER_PORT);

	networkP->init();

	LM_T(LmtInit, ("calling runUntilReady"));
	networkP->runUntilReady();
	LM_T(LmtInit, ("runUntilReady done"));

	mainWinCreate(qApp);
	tabManager = new TabManager(mainWindow);

	mainWindow->show();

	LM_T(LmtInit, ("letting control to QT main loop"));
	if (qtAppRunning == false)
	{
		qtAppRunning = true;
		qApp->exec();
	}

	return 0;
}
