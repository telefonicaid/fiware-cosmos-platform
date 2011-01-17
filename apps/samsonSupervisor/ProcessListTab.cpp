/* ****************************************************************************
*
* FILE                     ProcessListTab.cpp
*
* AUTHOR                   Ken Zangelin
*
* CREATION DATE            Jan 14 2011
*
*/
#include <QWidget>
#include <QLabel>
#include <QVBoxLayout>

#include "logMsg.h"             // LM_*

#include "globals.h"            // global vars
#include "actions.h"            // help, list, start, ...
#include "Starter.h"            // Starter
#include "Spawner.h"            // Spawner
#include "Process.h"            // Process
#include "ProcessListTab.h"     // Own interface



/* ****************************************************************************
*
* ProcessListTab::ProcessListTab - 
*/
ProcessListTab::ProcessListTab(const char* name, QWidget *parent) : QWidget(parent)
{
	unsigned int  noOfSpawners;
	Spawner**     spawnerVec;
	unsigned int  noOfProcesses;
	Process**     processVec;

	mainLayout = new QGridLayout(parent);

	setLayout(mainLayout);

	spawnerVec = spawnerListGet(&noOfSpawners);
	LM_M(("Got %d Spawners", noOfSpawners));
	spawnerListCreate(spawnerVec, noOfSpawners);

	processVec = processListGet(&noOfProcesses);
	LM_M(("Got %d Processes", noOfProcesses));
	processListCreate(processVec, noOfProcesses);

	memset(starterV, 0, sizeof(starterV));
}



/* ****************************************************************************
*
* spawnerListCreate - 
*/
void ProcessListTab::spawnerListCreate(Spawner** spawnerV, int spawners)
{
	Starter*       starter;
	Spawner*       spawnerP;
	ss::Endpoint*  ep;
	int            ix;
	int            column = 0;

	LM_M(("Creating %d spawners", spawners));

	QLabel* spawnersLabel = new QLabel("Spawners");
	mainLayout->addWidget(spawnersLabel, 0, column);

	for (ix = 0; ix < spawners; ix++)
	{
		if (spawnerV[ix] == NULL)
			continue;

		spawnerP = spawnerV[ix];
		starter  = new Starter("Spawner", spawnerP->host);

		LM_M(("Creating spawner '%s'", spawnerP->host));
		starter->spawnerSet(spawnerP);
		mainLayout->addWidget(starter->checkbox, ix + 1, column);
		starterAdd(starter);

		ep = networkP->endpointLookup(ss::Endpoint::Spawner, spawnerP->host);
		if (ep)
		{
			starter->connected  = true;
			starter->checkState = Qt::Checked;
		}
		else
		{
			starter->connected  = false;
			starter->checkState = Qt::Unchecked;
		}

		starter->checkbox->setCheckState(starter->checkState);
	}

	for (ix = 0; ix < 20; ix++)
		mainLayout->setRowMinimumHeight(ix, 40);

	// Connect Button
	connectButton = new QPushButton("Connect To All Spawners");
	connectButton->connect(connectButton, SIGNAL(clicked()), this, SLOT(connect()));
	mainLayout->addWidget(connectButton, 20, column);
}



/* ****************************************************************************
*
* processListCreate - 
*/
void ProcessListTab::processListCreate(Process** processV, int process)
{
	Starter*       starter;
	Process*       processP;
	ss::Endpoint*  ep;
	char           name[128];
	int            ix;
	int            column = 1;

	LM_M(("Creating %d process", process));

	QLabel* processLabel = new QLabel("Processes");
	mainLayout->addWidget(processLabel, 0, column);
	
	for (ix = 0; ix < process; ix++)
	{
		if (processV[ix] == NULL)
			continue;

		processP = processV[ix];
		snprintf(name, sizeof(name), "%s@%s", processP->name, processP->host);
		starter  = new Starter("Process", name);

		LM_M(("Creating process '%s'", processP->name));
		starter->processSet(processP);
		mainLayout->addWidget(starter->checkbox, ix + 1, column);

		if (strcmp(processP->name, "Controller") == 0)
			ep = networkP->endpointLookup(ss::Endpoint::Controller, processP->host);
		else if (strcmp(processP->name, "Worker") == 0)
			ep = networkP->endpointLookup(ss::Endpoint::Worker, processP->host);

		if (ep)
		{
			starter->connected  = true;
			starter->checkState = Qt::Checked;
		}
		else
		{
			starter->connected  = false;
			starter->checkState = Qt::Unchecked;
		}

		starter->checkbox->setCheckState(starter->checkState);
	}

	// Start Button
	startButton = new QPushButton("Start All Processes");
	startButton->connect(startButton, SIGNAL(clicked()), this, SLOT(start()));
	mainLayout->addWidget(startButton, 20, column);

	mainLayout->setColumnStretch(19, 500);
}



/* ****************************************************************************
*
* ProcessListTab::connect
*/
void ProcessListTab::connect(void)
{
	connectToAllSpawners();
}



/* ****************************************************************************
*
* ProcessListTab::start
*/
void ProcessListTab::start(void)
{
	startAllProcesses();
}



/* ****************************************************************************
*
* ProcessListTab::quit
*/
void ProcessListTab::quit(void)
{
	LM_X(0, ("Quit button pressed - I quit !"));
}



/* ****************************************************************************
*
* ProcessListTab::starterAdd - 
*/
void ProcessListTab::starterAdd(Starter* starter)
{
	unsigned int ix;

	for (ix = 0; ix < sizeof(starterV) / sizeof(starterV[0]); ix++)
	{
		if (starterV[ix] != NULL)
			continue;

		LM_M(("*** Adding '%s' starter '%s'", starter->type, starter->name));
		starterV[ix] = starter;
		return;
	}

	LM_X(1, ("No room for starters (vector size is %d)", sizeof(starterV) / sizeof(starterV[0])));
}



/* ****************************************************************************
*
* ProcessListTab::starterLookup - 
*/
Starter* ProcessListTab::starterLookup(ss::Endpoint* ep)
{
	unsigned int ix;
	char*        host;

	for (ix = 0; ix < sizeof(starterV) / sizeof(starterV[0]); ix++)
	{
		if (starterV[ix] == NULL)
			continue;

		LM_M(("*** Comparing %p to %p (%s)", ep, starterV[ix]->endpoint, starterV[ix]->endpoint->name.c_str()));
		if (starterV[ix]->endpoint == ep)
			return starterV[ix];
	}

	for (ix = 0; ix < sizeof(starterV) / sizeof(starterV[0]); ix++)
	{
		if (starterV[ix] == NULL)
			continue;

		LM_M(("***  Testing '%s' starter '%s'", starterV[ix]->type, starterV[ix]->name));

		if (starterV[ix]->spawner != NULL)
			host = starterV[ix]->spawner->host;
		else if (starterV[ix]->process != NULL)
			host = starterV[ix]->process->host;
		else
			continue;

		LM_M(("*** Comparing '%s' to '%s'", host, ep->ip.c_str()));
		if (strcmp(host, ep->ip.c_str()) == 0)
		{
			starterV[ix]->endpoint = ep;
			return starterV[ix];
		}
	}

	return NULL;
}
