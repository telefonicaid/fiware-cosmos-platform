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
#include "traceLevels.h"        // LMT_*

#include "globals.h"            // global vars
#include "actions.h"            // help, list, start, ...
#include "Starter.h"            // Starter
#include "Spawner.h"            // Spawner
#include "Process.h"            // Process
#include "spawnerList.h"        // spawnerListGet, ...
#include "processList.h"        // processListGet, ...
#include "starterList.h"        // starterAdd, ...
#include "ProcessListTab.h"     // Own interface



#define ROWS 12

/* ****************************************************************************
*
* ProcessListTab::ProcessListTab - 
*/
ProcessListTab::ProcessListTab(const char* name, QWidget *parent) : QWidget(parent)
{
	Starter**     starterV;
	unsigned int  starters;
	int           spawnerColumn = 0;
	int           processColumn = 1;

	mainLayout = new QGridLayout(parent);
	setLayout(mainLayout);

	startersCreate();

	QLabel* spawnersLabel = new QLabel("Spawners");
	mainLayout->addWidget(spawnersLabel, 0, spawnerColumn);

	QLabel* processesLabel = new QLabel("Processes");
	mainLayout->addWidget(processesLabel, 0, processColumn);

	starters  = starterMaxGet();
	starterV  = starterListGet();

	LM_T(LMT_PROCESS_LIST_TAB, ("Creating QT part of %d starters", starters));
	for (unsigned int ix = 0; ix < starters; ix++)
	{
		if (starterV[ix] == NULL)
			continue;

		LM_T(LMT_PROCESS_LIST_TAB, ("Creating checkbox for '%s'", starterV[ix]->name));
		starterV[ix]->checkbox = new QCheckBox(QString(starterV[ix]->name), starterV[ix]);

		if (starterV[ix]->type == Starter::SpawnerConnecter)
		{
			LM_T(LMT_PROCESS_LIST_TAB, ("Creating checkbox for spawner-starter '%s'", starterV[ix]->name));
			starterV[ix]->checkbox->connect(starterV[ix]->checkbox, SIGNAL(clicked()), starterV[ix], SLOT(spawnerClicked()));
			mainLayout->addWidget(starterV[ix]->checkbox, ix + 1, spawnerColumn);

			starterV[ix]->endpoint = networkP->endpointLookup(ss::Endpoint::Spawner, starterV[ix]->spawner->host);
		}
		else if (starterV[ix]->type == Starter::ProcessStarter)
		{
			LM_T(LMT_PROCESS_LIST_TAB, ("Creating checkbox for process-starter '%s'", starterV[ix]->name));
			starterV[ix]->checkbox->connect(starterV[ix]->checkbox, SIGNAL(clicked()), starterV[ix], SLOT(processClicked()));
			mainLayout->addWidget(starterV[ix]->checkbox, ix + 1, processColumn);

			if (strcmp(starterV[ix]->process->name, "Controller") == 0)
				starterV[ix]->endpoint = networkP->endpointLookup(ss::Endpoint::Controller, starterV[ix]->process->host);
			else if (strcmp(starterV[ix]->process->name, "Worker") == 0)
				starterV[ix]->endpoint = networkP->endpointLookup(ss::Endpoint::Worker, starterV[ix]->process->host);
		}
		else
			LM_X(1, ("bad type '%d' for Starter", starterV[ix]->type));

		starterV[ix]->check();
	}

	for (unsigned int ix = 0; ix < ROWS; ix++)
		mainLayout->setRowMinimumHeight(ix, 40);
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
* startersCreate - 
*/
void ProcessListTab::startersCreate(void)
{
	//
	// One Qt Starter for each Spawner
	//
	Spawner**     spawnerV;
	unsigned int  spawnerMax;

	spawnerV   = spawnerListGet();
	spawnerMax = spawnerMaxGet();

	for (unsigned int ix = 0; ix < spawnerMax; ix++)
	{
		if (spawnerV[ix] == NULL)
			continue;

		LM_T(LMT_STARTER, ("Adding starter for spawner in '%s'", spawnerV[ix]->host));
		starterAdd(spawnerV[ix]);
	}



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

		LM_T(LMT_STARTER, ("Adding starter for process '%s' in '%s'", processV[ix]->name, processV[ix]->host));
		starterAdd(processV[ix]);
	}
}
