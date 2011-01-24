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



#if 0
/* ****************************************************************************
*
* Accepter - 
*/
class Accepter : public QObject
{
public:
	int             listenFd;
	unsigned short  port;

	Accepter(int fd, unsigned short port)
	{
		this->listenFd = fd;
		this->port     = port;

		LM_M(("Initialized Accepter for fd %d (port %d)", this->listenFd, this->port));
		startTimer(50);  // 50 millisecond timer
	};

protected:
	void timerEvent(QTimerEvent* e)
	{
		struct timeval          tv;
		int                     fds;
		fd_set                  rFds;
		unsigned int            ix;
		LogProvider**           lpV;
		unsigned int            providers;
		int                     max;
		static struct timeval   lastTime = { 0, 0 };
		struct timeval          now;
		bool                    showList = false;

		gettimeofday(&now, NULL);
		if (now.tv_sec - lastTime.tv_sec >= 3)
		{
			lastTime = now;
			showList = true;
		}

		while (1)
		{

			tv.tv_sec  = 0;
			tv.tv_usec = 0;

			if (showList)
				LM_F((""));
			max = 0;
			do
			{
				lpV       = logProviderListGet();
				providers = logProviderMaxGet();

				if (lpV == NULL)
					LM_RVE(("No providers found"));

				FD_ZERO(&rFds);

				FD_SET(listenFd, &rFds);
				max = listenFd;

				if (showList)
					LM_F(("+ fd: %02d (listen socket)", listenFd));

				for (ix = 0; ix < providers; ix++)
				{
					if (lpV[ix] == NULL)
						continue;

					if (lpV[ix]->fd == -1)
						continue;

					FD_SET(lpV[ix]->fd, &rFds);
					max = MAX(max, lpV[ix]->fd);

					if (showList)
						LM_F(("+ %02d (%s@%s)", lpV[ix]->fd, lpV[ix]->name, lpV[ix]->host));
				}

				fds = select(max + 1, &rFds, NULL, NULL, &tv);
			} while ((fds == -1) && (errno == EINTR));

			if (showList)
			{
				LM_M(("---------------------- %d fds ready --------------------", fds));
				LM_M((""));
			}

			if (fds == -1)
				LM_E(("select: %s", strerror(errno)));
			else if (fds == 0)
				return;
			else if (FD_ISSET(listenFd, &rFds))
			{
				char  ip[128];
				int   fd;

				LM_M(("calling iomAccept(listenFd == %d)", listenFd));
				fd = iomAccept(listenFd, ip, sizeof(ip));
				if (fd == -1)
					LM_E(("iomAccept: %s", strerror(errno)));
				else
				{
					ss::Message::HelloData   hello;

					hello.type     = ss::Endpoint::LogServer;
					hello.workers  = 0;
					hello.port     = LOG_SERVER_PORT;
					hello.coreNo   = -1;
					hello.workerId = -1;

					strncpy(hello.name,   progName,     sizeof(hello.name));
					strncpy(hello.ip,     ipGet(),      sizeof(hello.ip));
					strncpy(hello.alias,  "logServer",  sizeof(hello.alias));

					iomMsgSend(fd, "connectingProcess", "logServer", ss::Message::Hello, ss::Message::Msg, &hello, sizeof(hello));
					logProviderAdd(NULL, "noname", ip, fd);
					// Perhaps I should wait to add provider until Hello Ack is received ...
				}
			}
			else
			{
				for (ix = 0; ix < providers; ix++)
				{
					if (lpV[ix] == NULL)
						continue;

					if (FD_ISSET(lpV[ix]->fd, &rFds))
					{
						FD_CLR(lpV[ix]->fd, &rFds);
						logProviderMsgTreat(lpV[ix]);
					}
				}
			}

			showList = false;
		}
	}
};
#endif



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

	paConfig("prefix",                        (void*) "SSS_");
	paConfig("usage and exit on any warning", (void*) true);
	paConfig("log to screen",                 (void*) "only errors");
	paConfig("log file line format",          (void*) "TYPE:DATE:EXEC-AUX/FILE[LINE] FUNC: TEXT");
	paConfig("screen line format",            (void*) "TYPE:EXEC: TEXT");
	paConfig("log to file",                   (void*) true);

	paParse(paArgs, argC, (char**) argV, 1, false);

	LM_F(("Started with arguments:"));
	for (int ix = 0; ix < argC; ix++)
		LM_F(("  %02d: '%s'", ix, argV[ix]));

	LM_M(("Initializing log provider list"));
	logProviderListInit(50);
	logWinCreate(&app);

	networkP        = new ss::Network(endpoints, 10); // 10 workers by default
	samsonLogServer = new SamsonLogServer();

    networkP->setEndpointUpdateReceiver(samsonLogServer);
    networkP->setReadyReceiver(samsonLogServer);
    networkP->setDataReceiver(samsonLogServer);

	networkP->init(ss::Endpoint::LogServer, "LogServer", port, controller);

	LM_M(("controller: '%s'", controller));
	if (strcmp(controller, (char*) NOC) != 0)
	{
		int fd;

		LM_M(("Connecting to controller at '%s'", controller));
		fd = iomConnect(controller, CONTROLLER_PORT);
		networkP->endpointAdd("Connecting to controller", fd, fd, "Controller", "controller", 0, ss::Endpoint::Controller, controller, CONTROLLER_PORT);
	}

	LM_M(("Number of spawners: %d", (int) spawnerList[0]));
	if ((int) spawnerList[0] != 0)
	{
		int ix;

		for (ix = 1; ix <= (int) spawnerList[0]; ix++)
		{
			int fd;

			LM_M(("Connecting to spawner %d: '%s'", ix, spawnerList[ix]));
			fd = iomConnect(spawnerList[ix], SPAWNER_PORT);
			if (fd != -1)
			{
				ss::Endpoint* ep = networkP->endpointAdd("Connecting to spawner", fd, fd, "Spawner", "spawner", 0, ss::Endpoint::Temporal, spawnerList[ix], SPAWNER_PORT);
				logProviderAdd(ep, "spawner", spawnerList[ix], fd);
			}
		}
	}

#if 0
	LM_M(("Number of workers: %d", (int) workerList[0]));
	if ((int) workerList[0] != 0)
	{
		int ix;

		for (ix = 1; ix <= (int) workerList[0]; ix++)
		{
			int fd;

			LM_M(("Connecting to worker %d: '%s'", ix, workerList[ix]));
			fd = iomConnect(workerList[ix], WORKER_PORT);
			if (fd != -1)
				logProviderAdd(NULL, "worker", workerList[ix], fd);
		}
	}
#endif

#if 0
	LM_M(("calling networkP->runUntilReady"));
	networkP->runUntilReady();
	LM_M(("back from networkP->runUntilReady"));
#endif

	LM_M(("Letting control to QT"));
	app.exec();

	return 0;
}
