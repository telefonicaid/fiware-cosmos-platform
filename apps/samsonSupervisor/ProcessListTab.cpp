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
