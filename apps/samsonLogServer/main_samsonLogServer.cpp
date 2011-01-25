/* ****************************************************************************
*
* FILE                     main_samsonLogServer.cpp
*
* AUTHOR                   Ken Zangelin
*
* CREATION DATE            Jan 20 2011
*
*/
#include <unistd.h>             // fork & exec

#include <QObject>
#include <QSocketNotifier>
#include <QListWidget>
#include <QVBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QDesktopWidget>
#include <QApplication>
#include <QDialog>
#include <QTimerEvent>

#include "logMsg.h"             // LM_*
#include "traceLevels.h"        // LMT_* 
#include "parseArgs.h"          // parseArgs

#include "ports.h"              // LOG_SERVER_PORT
#include "Message.h"            // ss::Message
#include "iomServerOpen.h"      // iomServerOpen
#include "iomAccept.h"          // iomAccept
#include "iomMsgSend.h"         // iomMsgSend
#include "iomMsgAwait.h"        // iomMsgAwait
#include "iomConnect.h"         // iomConnect
#include "logProviderList.h"    // logProviderListInit, logProviderAdd, ...
#include "SamsonLogServer.h"    // SamsonLogServer



/* ****************************************************************************
*
* Window geometry
*/
#define LOG_WIN_WIDTH     1200
#define LOG_WIN_HEIGHT     300



/* ****************************************************************************
*
* Option variables
*/
unsigned short   port;
int              endpoints;
char             controller[80];
char*            workerList[32];
char*            spawnerList[32];



#define NOC (long int) "no controller"
/* ****************************************************************************
*
* parse arguments
*/
PaArgument paArgs[] =
{
	{ "-port",        &port,        "PORT",         PaShortU,  PaOpt,  1232,   1025,  65000,  "listen port"                              },
	{ "-endpoints",   &endpoints,   "ENDPOINTS",    PaInt,     PaOpt,    80,      3,    100,  "number of endpoints"                      },
	{ "-controller",   controller,  "CONTROLLER",   PaString,  PaOpt,   NOC,   PaNL,   PaNL,  "controller IP"                            },
	{ "-workerList",   workerList,  "WORKER_LIST",  PaSList,   PaOpt,  PaND,   PaNL,   PaNL,  "list of hosts for workers to connect to"  },
	{ "-spawnerList",  spawnerList, "SPAWNER_LIST", PaSList,   PaOpt,  PaND,   PaNL,   PaNL,  "list of hosts for spawners to connect to" },
	
	PA_END_OF_ARGS
};



/* ****************************************************************************
*
* logFd - file descriptor for log file used in all libraries
*/
int logFd = -1;



/* ****************************************************************************
*
* mainWindow - 
*/
QDesktopWidget*   desktop         = NULL;
QWidget*          logWindow       = NULL;
QVBoxLayout*      mainLayout      = NULL;
QLabel*           idleLabel       = NULL;
ss::Network*      networkP        = NULL;
SamsonLogServer*  samsonLogServer = NULL;



/* ****************************************************************************
*
* MAX - 
*/
#ifndef MAX
#define MAX(a, b)  ( ((a) > (b))? (a) : (b))
#endif



/* ****************************************************************************
*
* wordClean - to be removed when LogServer uses Network
*/
static char* wordClean(char* str)
{
	char* endP;

	while ((*str == ' ') || (*str == '\t'))
		++str;

	endP = str;
	while ((*endP != 0) && (*endP != ' ') && (*endP != '\t'))
		++endP;
	*endP = 0;

	return str;
}



/* ****************************************************************************
*
* ipGet - to be removed when LogServer uses Network
*/
char* ipGet(void)
{
	char  line[80];
	char* ipP = (char*) "II.PP";
	FILE* fP;
	
	fP = popen("ifconfig | grep \"inet addr:\" | awk -F: '{ print $2 }' | awk '{ print $1 }'", "r");
	if (fgets(line, sizeof(line), fP) != NULL)
	{
		if (line[strlen(line) - 1] == '\n')
			line[strlen(line) - 1] = 0;
		ipP = wordClean(line);
	}

	fclose(fP);

	return strdup(ipP);
}



/* ****************************************************************************
*
* logWinCreate - 
*/
static void logWinCreate(QApplication* app)
{
	int              screenWidth;
	int              screenHeight;
	int              x;
	int              y;
	
	mainLayout = new QVBoxLayout;
	desktop    = QApplication::desktop();
	logWindow  = new QWidget();

	// Window Geometry
	screenWidth  = desktop->width();
	screenHeight = desktop->height();

	x = (screenWidth  - LOG_WIN_WIDTH)  / 2;
	y = (screenHeight - LOG_WIN_HEIGHT) / 2;

	logWindow->resize(LOG_WIN_WIDTH, LOG_WIN_HEIGHT);
	logWindow->move(x, y);
	logWindow->setWindowTitle("Samson Supervisor Log");
	
	idleLabel = new QLabel("Accepting connections");
	QFont idleFont("Times", 30, QFont::Normal);

	idleLabel->setFont(idleFont);
	mainLayout->addWidget(idleLabel);
    mainLayout->addStretch(30);

	logWindow->setLayout(mainLayout);
	logWindow->show();
}



/* ****************************************************************************
*
* main - 
*/
int main(int argC, const char *argV[])
{
	QApplication   app(argC, (char**) argV);
	pid_t          pid;

	// fork once and make father die - this way samsonLogServer will have
	// process 1 (init) as father and samsonSupervisor wont receive SIGCHLD
	// nor have to wait for it to avoid a Zombie.
	// 
	// What samsonSupervisor WILL have to do is to wait for the LogServer
	// father process to die, but as this happens at startup of LogServer
	// this is simple enough ...

	pid = fork();
	if (pid != 0)
		exit(0);

	paConfig("prefix",                        (void*) "SSS_");
	paConfig("usage and exit on any warning", (void*) true);
	paConfig("log to screen",                 (void*) "only errors");
	paConfig("log file line format",          (void*) "TYPE:DATE:EXEC-AUX/FILE[LINE] FUNC: TEXT");
	paConfig("screen line format",            (void*) "TYPE:EXEC: TEXT");
	paConfig("log to file",                   (void*) true);

	paParse(paArgs, argC, (char**) argV, 1, false);

	LM_T(LmtInit, ("Command line arguments:"));
	for (int ix = 0; ix < argC; ix++)
		LM_T(LmtInit, ("  %02d: '%s'", ix, argV[ix]));

	LM_T(LmtInit, ("Initializing log provider list"));
	logProviderListInit(50);
	logWinCreate(&app);

	LM_T(LmtInit, ("creating Network"));
	networkP        = new ss::Network(endpoints, 10); // 10 workers by default

	LM_T(LmtInit, ("creating SamsonLogServer"));
	samsonLogServer = new SamsonLogServer();

	LM_T(LmtInit, ("setting up Network callbacks"));
    networkP->setEndpointUpdateReceiver(samsonLogServer);
    networkP->setReadyReceiver(samsonLogServer);
    networkP->setDataReceiver(samsonLogServer);

	LM_T(LmtInit, ("initializing Network"));
	networkP->init(ss::Endpoint::LogServer, "LogServer", port, controller);

	if (strcmp(controller, (char*) NOC) != 0)
	{
		int fd;

		LM_T(LmtInit, ("Connecting to controller at '%s'", controller));
		fd = iomConnect(controller, CONTROLLER_PORT);
		LM_T(LmtInit, ("Adding endpoint for controller"));
		networkP->endpointAdd("Connecting to controller", fd, fd, "Controller", "controller", 0, ss::Endpoint::Controller, controller, CONTROLLER_PORT);
		LM_T(LmtInit, ("Added endpoint for controller"));
	}
	else
		LM_T(LmtInit, ("Not connecting to Controller"));

	LM_T(LmtInit, ("Number of spawners: %d", (int) spawnerList[0]));
	if ((int) spawnerList[0] != 0)
	{
		int ix;

		for (ix = 1; ix <= (int) spawnerList[0]; ix++)
		{
			int fd;

			LM_T(LmtInit, ("Connecting to spawner %d: '%s'", ix, spawnerList[ix]));
			fd = iomConnect(spawnerList[ix], SPAWNER_PORT);
			if (fd != -1)
			{
				ss::Endpoint* ep;

				LM_T(LmtInit, ("Adding endpoint for spawner '%s'", spawnerList[ix]));
				ep = networkP->endpointAdd("Connecting to spawner", fd, fd, "Spawner", "spawner", 0, ss::Endpoint::Temporal, spawnerList[ix], SPAWNER_PORT);
				LM_T(LmtInit, ("Adding logProvider for spawner '%s'", spawnerList[ix]));
				logProviderAdd(ep, "spawner", spawnerList[ix], fd);
			}
			else
				LM_T(LmtInit, ("Error connecting to spawner %d: '%s'", ix, spawnerList[ix]));
		}
		LM_T(LmtInit, ("Connected to all spawners"));
	}
	else
		LM_T(LmtInit, ("Not connecting to Spawners"));



#if 0
	LM_T(LmtInit, ("Number of workers: %d", (int) workerList[0]));
	if ((int) workerList[0] != 0)
	{
		int ix;

		for (ix = 1; ix <= (int) workerList[0]; ix++)
		{
			int fd;

			LM_T(LmtInit, ("Connecting to worker %d: '%s'", ix, workerList[ix]));
			fd = iomConnect(workerList[ix], WORKER_PORT);
			if (fd != -1)
				logProviderAdd(NULL, "worker", workerList[ix], fd);
		}
	}
#endif

#if 0
	LM_T(LmtInit, ("calling networkP->runUntilReady"));
	networkP->runUntilReady();
	LM_T(LmtInit, ("back from networkP->runUntilReady"));
#endif

	LM_T(LmtInit, ("Letting control to QT"));
	app.exec();

	return 0;
}
