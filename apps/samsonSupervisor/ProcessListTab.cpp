/* ****************************************************************************
*
* FILE                     ProcessListTab.cpp
*
* AUTHOR                   Ken Zangelin
*
* CREATION DATE            Jan 14 2011
*
*/
#include <sys/types.h>          // pid_t
#include <sys/wait.h>           // waitpid

#include <QWidget>
#include <QLabel>
#include <QVBoxLayout>

#include "logMsg.h"             // LM_*
#include "traceLevels.h"        // Trace Levels

#include "Endpoint.h"           // ss::Endpoint
#include "Network.h"            // samsonWorkerEndpoints
#include "globals.h"            // global vars
#include "ports.h"              // ports ...
#include "iomConnect.h"         // iomConnect
#include "actions.h"            // help, list, start, ...
#include "Starter.h"            // Starter
#include "Process.h"            // Process
#include "spawnerList.h"        // spawnerListGet, ...
#include "processList.h"        // processListGet, ...
#include "starterList.h"        // starterAdd, ...
#include "Popup.h"              // Popup
#include "ProcessListTab.h"     // Own interface



/* ****************************************************************************
*
* global vars
*/
static int                 spawnerColumn = 0;
static int                 processColumn = 3;
static int                 spawnerRow    = 1;
static int                 processRow    = 1;



#define ROWS 12
/* ****************************************************************************
*
* ProcessListTab::ProcessListTab - 
*/
ProcessListTab::ProcessListTab(const char* name, QWidget *parent) : QWidget(parent)
{
	Starter**           starterV;
	unsigned int        starters;
	const unsigned int  Columns       = 5;

	mainLayout = new QGridLayout(parent);
	setLayout(mainLayout);

	initialStartersCreate();

	QLabel* spawnersLabel = new QLabel("Spawners");
	mainLayout->addWidget(spawnersLabel, 0, spawnerColumn);

	QLabel* processesLabel = new QLabel("Processes");
	mainLayout->addWidget(processesLabel, 0, processColumn);

	starters  = starterMaxGet();
	starterV  = starterListGet();

	LM_T(LmtProcessListTab, ("Creating QT part of %d starters", starters));
	networkP->endpointListShow("Creating QT part for starters");

	for (unsigned int ix = 0; ix < starters; ix++)
	{
		if (starterV[ix] == NULL)
			continue;

		starterInclude(starterV[ix]);
	}



	//
	// Log Server start button
	//
	logServerRunningLabel = new QLabel("Log Server Running");
	logServerStartButton  = new QPushButton("Start Log Server");
	
	mainLayout->addWidget(logServerStartButton,  ROWS, spawnerColumn);
	mainLayout->addWidget(logServerRunningLabel, ROWS, spawnerColumn);

	logServerStartButton->connect(logServerStartButton, SIGNAL(clicked()), this, SLOT(logServerStart()));

	ss::Endpoint* logServerEndpoint = networkP->endpointLookup((char*) "logServer");
	if (logServerEndpoint)
	{
		logServerRunningLabel->show();
		logServerStartButton->hide();
	}
	else
	{
		logServerRunningLabel->hide();
		logServerStartButton->show();
	}

	for (unsigned int ix = 0; ix < ROWS; ix++)
		mainLayout->setRowMinimumHeight(ix, 40);

	for (unsigned int ix = 0; ix < Columns; ix++)
		mainLayout->setColumnMinimumWidth(ix, 100);
}



/* ****************************************************************************
*
* ProcessListTab::quit - 
*/
void ProcessListTab::quit(void)
{
	LM_X(0, ("Quit button pressed - I quit !"));
}



/* ****************************************************************************
*
* ProcessListTab::logServerStart - 
*/
void ProcessListTab::logServerStart(void)
{
	pid_t pid;
	int   tries = 0;

	if ((pid = fork()) == 0)
	{
		std::vector<ss::Endpoint*>  workerV;
		std::vector<ss::Endpoint*>  spawnerV;
		unsigned int                ix;
		char*                       argV[100];
		int                         argC = 0;
		char                        controllerIp[128];
		char                        workers[1024];
		char                        spawners[1024];
		ss::Endpoint*               controller;

		argV[0] = (char*) "samsonLogServer";
		argC    = 1;

		controller = networkP->controllerGet();

		if (controller != NULL)
		{
			snprintf(controllerIp, sizeof(controllerIp), "%s", controller->ip.c_str());
			argV[argC++] = (char*) "-controller";
			argV[argC++] = controllerIp;
		}

		workerV = networkP->samsonWorkerEndpoints();
		LM_T(LmtWorkers, ("samsonWorkerEndpoints returned a list of %d workers", workerV.size()));

		memset(workers, 0, sizeof(workers));
		if (workerV.size() > 0)
		{
			argV[argC++] = (char*) "-workerList";

			for (ix = 0; ix < workerV.size(); ix++)
			{
				LM_T(LmtWorkers, ("Adding worker '%s'", workerV[ix]->ip.c_str()));
				strcat(workers, workerV[ix]->ip.c_str());
				strcat(workers, " ");
			}
			argV[argC++] = workers;
		}

		spawnerV = networkP->samsonEndpoints(ss::Endpoint::Spawner);
		LM_T(LmtEndpoints, ("samsonEndpoints(Spawner) returned a list of %d spawner endpoints", spawnerV.size()));
		memset(spawners, 0, sizeof(spawners));
		if (spawnerV.size() > 0)
		{
			argV[argC++] = (char*) "-spawnerList";

			for (ix = 0; ix < spawnerV.size(); ix++)
			{
				LM_T(LmtSpawner, ("Adding spawner '%s'", spawnerV[ix]->ip.c_str()));
				strcat(spawners, spawnerV[ix]->ip.c_str());
				strcat(spawners, " ");
			}
			argV[argC++] = spawners;
		}

		argV[argC] = NULL;

		LM_T(LmtLogServer, ("Execing logServer with controller: '%s', spawners '%s' and workers: '%s'", controllerIp, spawners, workers));
		for (int ix = 0; ix < argC; ix++)
			LM_T(LmtInit, ("argV[%d]: '%s'", ix, argV[ix]));

		execvp(argV[0], argV);
		LM_X(1, ("Back from exec ..."));
	}

	LM_T(LmtLogServer, ("Awaiting dead father of log server (log server does fork and the father exits at startup"));
	int status;
	waitpid(pid, &status, 0);
	LM_T(LmtLogServer, ("Dead father of log server waited for"));

	LM_T(LmtLogServer, ("Connecting to log server"));
	usleep(100000);
	int logServerFd = -1;

	networkP->endpointListShow("connecting to log server");

	while (1)
	{
		LM_T(LmtLogServer, ("trying to connect to log server"));
		logServerFd = iomConnect("localhost", LOG_SERVER_PORT);
		if (logServerFd != -1)
			break;
		usleep(100000);
		if (++tries > 10)
			break;
	}

	if (logServerFd == -1)
		new Popup("Error", "Error connecting to Samson Log Server");
	else
	{
		logServerEndpoint = networkP->endpointLookup((char*) "logServer");
		if (logServerEndpoint != NULL)
		{
			logServerEndpoint->rFd   = logServerFd;
			logServerEndpoint->wFd   = logServerFd;
			logServerEndpoint->state = ss::Endpoint::Connected;
		}
		else
		{
			LM_T(LmtLogServer, ("logServerEndpoint not found ..."));
			logServerEndpoint = networkP->endpointAdd("logServerEndpoint not found", logServerFd, logServerFd, "Samson Log Server", "logServer", 0, ss::Endpoint::Temporal, "localhost", LOG_SERVER_PORT);
		}
	}
}



/* ****************************************************************************
*
* initialStartersCreate - 
*/
void ProcessListTab::initialStartersCreate(void)
{
	//
	// One Qt Starter for each Process
	//
	Process**     processV;
	unsigned int  processMax;

	processV   = processListGet();
	processMax = processMaxGet();

	for (unsigned int ix = 0; ix < processMax; ix++)
	{
		if (processV[ix] == NULL)
			continue;

		if (processV[ix]->starterP != NULL)
		{
			LM_T(LmtStarter, ("NOT Adding starter for process '%s' in '%s' - it already exists!", processV[ix]->name, processV[ix]->host));
			continue;
		}

		LM_T(LmtStarter, ("Adding starter for process '%s' in '%s'", processV[ix]->name, processV[ix]->host));
		starterAdd(processV[ix]);
	}
}



/* ****************************************************************************
*
* starterInclude - 
*/
void ProcessListTab::starterInclude(Starter* starterP)
{
	if (starterP->process->type == PtSpawner)
		starterP->qtInit(mainLayout, spawnerRow++, spawnerColumn);
	else
		starterP->qtInit(mainLayout, processRow++, processColumn);
}
