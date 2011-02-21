/* ****************************************************************************
*
* FILE                     main_samsonSupervisor.cpp
*
* AUTHOR                   Ken Zangelin
*
* CREATION DATE            Dec 14 2010
*
*/
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

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
#include "permissions.h"        // Permissions
#include "LoginWindow.h"        // LoginWindow
#include "SamsonSetup.h"		// ss::SamsonSetup
#include "MemoryManager.h"      // ss::MemoryManager
#include "Delilah.h"            // ss::Delilah
#include "DelilahConsole.h"     // ss::DelilahConsole



/* ****************************************************************************
*
* Window geometry
*/
#define MAIN_WIN_WIDTH     1400
#define MAIN_WIN_HEIGHT     800



/* ****************************************************************************
*
* Definitions
*/
#define MEGAS(mbs) (1024 * 1024 * (mbs))
#define GIGAS(gbs) (1024 * MEGAS(gbs))



/* ****************************************************************************
*
* Global variables
*/
int                  logFd             = -1;    // file descriptor for log file
bool                 qtAppRunning      = false;
ss::Network*         networkP          = NULL;
SamsonSupervisor*    supervisorP       = NULL;
ss::Endpoint*        controller        = NULL;
TabManager*          tabManager        = NULL;
QWidget*             mainWindow        = NULL;
QDesktopWidget*      desktop           = NULL;
ConnectionMgr*       connectionMgr     = NULL;
QueueMgr*            queueMgr          = NULL;
SourceMgr*           sourceMgr         = NULL;
ResultMgr*           resultMgr         = NULL;
UserMgr*             userMgr           = NULL;
User*                userP             = NULL;
int                  mainWinWidth      = MAIN_WIN_WIDTH;
int                  mainWinHeight     = MAIN_WIN_HEIGHT;
InfoWin*             infoWin           = NULL;
char*                controllerName    = NULL;



/* ****************************************************************************
*
* Option variables
*/
char    controllerHost[80];
bool    kz;
bool    andreu;
bool    usecss;
int     workers;



#define CFP (long int)  "/opt/samson/etc/platformProcesses"
#define LOC  (long int) "localhost"
/* ****************************************************************************
*
* Parse arguments
*/
PaArgument paArgs[] =
{
	{ "-controller",  controllerHost,  "CONTROLLER",    PaString,  PaOpt,   LOC,  PaNL,   PaNL,  "controller IP"       },
	{ "-workers",     &workers,        "WORKERS",       PaInt,     PaOpt,     5,     1,     20,  "number of workers"   },
	{ "-kz",          &kz,             "KZ_LOGIN",      PaBool,    PaHid, false, false,   true,  "login as 'kz'"       },
	{ "-andreu",      &andreu,         "ANDREU_LOGIN",  PaBool,    PaHid, false, false,   true,  "login as 'andreu'"   },
	{ "-usecss",      &usecss,         "USE_CSS",       PaBool,    PaHid, false, false,   true,  "use css"             },

	PA_END_OF_ARGS
};



/* ****************************************************************************
*
* Global variables
*/
int     endpoints = 80;
char*   controllerHostP = &controllerHost[0];



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
* setStyleSheet - 
*/
void setStyleSheet(const char* fileName)
{
	int          fd;
	char         buf[248 * 1024];
	int          nb;
	int          tot;
	struct stat  statBuf;

	if (stat((char*) fileName, &statBuf) == -1)
		LM_RVE(("stat(%s): %s - no style sheet ...", fileName, strerror(errno)));

	if ((int) statBuf.st_size > (int) sizeof(buf))
		LM_X(1, ("Increase buffer size for style sheet. File '%s' bigger than buffer ...", fileName));

	fd  = open(fileName, O_RDONLY);
	if (fd == -1)
		LM_RVE(("Error opening style sheetfile '%s': %s", fileName, strerror(errno)));

	tot = 0;
	while (tot < statBuf.st_size)
	{
		nb = read(fd, &buf[tot], statBuf.st_size - tot);

		if (nb == -1)
			LM_RVE(("read(%s): %s", fileName, strerror(errno)));

		tot += nb;
	}

	qApp->setStyleSheet(buf);
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
* userInit - 
*/
static void userInit(void)
{
	userMgr = new UserMgr(10);

	userMgr->insert("superman", "samsonite", UpAll);
	userMgr->insert("guest",    "",          UpNothing);
	userMgr->insert("kz",       "kz",        UpAll);
	userMgr->insert("au",       "au",        UpAll);
	userMgr->insert("3rdParty", "please",    UpStartProcesses | UpStopProcesses | UpSeeLogs);

	if (kz == true)
		userP = userMgr->lookup("kz");
	else if (andreu == true)
		userP = userMgr->lookup("au");
	else
		login();

	if (userP)
		LM_T(LmtUser, ("Logged in as user '%s'", userP->name));
}



/* ****************************************************************************
*
* networkPrepare - 
*/
static void networkPrepare(void)
{
	LM_T(LmtInit, ("calling ss::Network with %d endpoints", endpoints));
	networkP    = new ss::Network(ss::Endpoint::Supervisor, "Supervisor", 0, endpoints);
	supervisorP = new SamsonSupervisor(networkP);

	networkP->setDataReceiver(supervisorP);
	networkP->setEndpointUpdateReceiver(supervisorP);
	networkP->setReadyReceiver(supervisorP);
}



/* ****************************************************************************
*
* spawnerConnect - 
*/
static void spawnerConnect(char* host)
{
	int fd;

	LM_T(LmtInit, ("Trying to connect to spawner in '%s'", host));
	fd = iomConnect(host, SPAWNER_PORT);

	if (fd == -1)
	{
		char title[128];
		char message[256];

		snprintf(title, sizeof(title), "Error connecting to Spawner process in '%s'", host);
		snprintf(message, sizeof(message), "Can't connect to controller's Spawner at %s, port %d\nPlease start all spawners before running this application", host, SPAWNER_PORT);
		new Popup(title, message, true);
		qApp->exec();
		exit(2);
	}

	LM_T(LmtInit, ("Connected to spawner"));
}



/* ****************************************************************************
*
* controllerConnect
*/
static void controllerConnect(char* host)
{
	ss::Endpoint*  controller;
	ss::Process*   processP;

	LM_T(LmtInit, ("Connecting to controller in '%s'", host));
	controller = networkP->controllerConnect(host);

#if 0
	if (controller->state != ss::Endpoint::Connected)
	{
		char   eText[256];
		char*  argVec[20];
		int    args        = 0;

		LM_TODO(("Not connected to controller - ask the config file about its command line options - and fill LogConfig Window accordingly"));

		args = 20;
		memset(argVec, 0, sizeof(argVec));

		if (configFileParse(host, "Controller", &args, argVec) == -1)
		{
			snprintf(eText,
					 sizeof(eText),
					 "Unable to connect to controller in '%s'.\nAlso unable to find info on Controller in config file.\nUnable to connect to Controller, sorry.",
					 host);
			new Popup("Cannot connect to Controller", eText, true);
		}
		else
		{
			LM_W(("configFileParse returned %d args for 'Controller'", args));
			snprintf(eText, sizeof(eText), "Can't connect to controller at %s, port %d", controller->ip.c_str(), controller->port);
			
			new Popup("Error connecting to Controller", eText, false);
		}
	}
#endif

	processP           = processAdd("Controller", host, CONTROLLER_PORT, "Controller", controller);
	processP->spawnerP = spawnerAdd("Spawner", host, SPAWNER_PORT);
}



/* ****************************************************************************
*
* delilahInit - prepare to send commands to Controller
*/
static void delilahInit(void)
{
	ss::SamsonSetup::load();
	ss::SamsonSetup::shared()->memory           = (size_t) GIGAS(1);
	ss::SamsonSetup::shared()->load_buffer_size = (size_t) MEGAS(64);
	ss::MemoryManager::init();

}



/* ****************************************************************************
*
* parseArgs - 
*/
static void parseArgs(int argC, char** argV)
{
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
}



/* ****************************************************************************
*
* networkInit - 
*/
static void networkInit(char* host)
{
	LM_T(LmtInit, ("Initializing Network (controller in %s)", host));
	networkP->init(host);
	networkP->runUntilReady();
	LM_T(LmtInit, ("runUntilReady done"));
}



/* ****************************************************************************
*
* qtGo - 
*/
static void qtGo(void)
{
	mainWinCreate(qApp);
	tabManager = new TabManager(mainWindow);

	if (usecss == true)
		setStyleSheet("/mnt/sda9/kzangeli/sb/samson/20/apps/samsonSupervisor/samson.css");

	mainWindow->show();

	qtAppRunning = true;
	qApp->exec();

	LM_W(("After QT main loop - Popup problem ... ?"));
}



/* ****************************************************************************
*
* controllerHostName - 
*/
static char* controllerHostName(void)
{
	Host* hostP;

	hostP = networkP->hostMgr->lookup(controllerHost);

	if (hostP != NULL)
		return hostP->name;

	return controllerHost;
}



/* ****************************************************************************
*
* main - 
*/
int main(int argC, const char *argV[])
{
	QApplication   app(argC, (char**) argV);

	Q_INIT_RESOURCE(samsonSupervisor);

	parseArgs(argC, (char**) argV);
	userInit();
	configFileInit("/opt/samson/etc/platformProcesses");
	processListInit(20);
	starterListInit(30);
	networkPrepare();
	controllerName = controllerHostName();
	spawnerConnect(controllerName);
	controllerConnect(controllerName);
	networkInit(controllerName);
	delilahInit();
	qtGo();

	return 0;
}
