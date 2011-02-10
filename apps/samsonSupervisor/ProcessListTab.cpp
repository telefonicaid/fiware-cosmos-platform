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
#include "ProcessConfigView.h"  // ProcessConfigView
#include "ProcessListTab.h"     // Own interface



/* ****************************************************************************
*
* ProcessListTab::ProcessListTab - 
*/
ProcessListTab::ProcessListTab(const char* name, QWidget *parent) : QWidget(parent)
{
	Starter**           starterV;
	unsigned int        starters;
	QVBoxLayout*        leftBasicLayout;

	mainLayout       = new QHBoxLayout(parent);
	leftBasicLayout  = new QVBoxLayout();
	leftLayout       = new QVBoxLayout();
	rightLayout      = new QVBoxLayout();
	rightGrid        = new QGridLayout();

	mainLayout->addLayout(leftBasicLayout);
	mainLayout->addStretch(10);
	mainLayout->addLayout(rightLayout);
	mainLayout->addStretch(100);

	leftBasicLayout->addLayout(leftLayout);
	leftBasicLayout->addStretch(100);

	rightLayout->addLayout(rightGrid);
	setLayout(mainLayout);

	configView = NULL;

	initialStartersCreate();

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
*
* One Qt Starter for each Process.
*/
void ProcessListTab::initialStartersCreate(void)
{
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
	starterP->qtInit(leftLayout);
}



/* ****************************************************************************
*
* configShow - 
*/
void ProcessListTab::configShow(Starter* starterP)
{
	ss::Endpoint* endpoint;

	endpoint = starterP->process->endpoint;
	if ((endpoint == NULL) || (endpoint->state != ss::Endpoint::Connected))
		new Popup("Not Implemented", "Please implement a way to configure a non running process.\nJust being able to select all command line options.");
	else
	{
		if (configView != NULL)
		{
			delete configView;
			configView = NULL;
		}

		configView = new ProcessConfigView(rightGrid, starterP->process);
	}
}
