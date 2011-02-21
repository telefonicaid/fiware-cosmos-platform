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

#include "globals.h"            // global vars
#include "ports.h"              // ports ...
#include "iomConnect.h"         // iomConnect
#include "iomMsgSend.h"         // iomMsgSend
#include "Network.h"            // samsonWorkerEndpoints
#include "Endpoint.h"           // ss::Endpoint
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
	controllerLayout = new QVBoxLayout();
	workerLayout     = new QVBoxLayout();
	spawnerLayout    = new QVBoxLayout();

	rightLayout      = new QVBoxLayout();
	righterLayout    = new QVBoxLayout();
	rightGrid        = new QGridLayout();
	
	mainLayout->addLayout(leftBasicLayout);
	mainLayout->addStretch(10);
	mainLayout->addLayout(rightLayout);
	mainLayout->addLayout(righterLayout);
	mainLayout->addStretch(100);

	leftBasicLayout->addLayout(leftLayout);
	leftBasicLayout->addStretch(100);

	leftLayout->addLayout(controllerLayout);
	leftLayout->addSpacing(50);
	leftLayout->addLayout(workerLayout);
	leftLayout->addSpacing(50);
	leftLayout->addLayout(spawnerLayout);
	leftLayout->addStretch(200);

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
	starterP->qtInit(spawnerLayout, workerLayout, controllerLayout);
}



Process*     processToBeConfigured          = NULL;
QGridLayout* gridForProcessToBeConfigured   = NULL;
/* ****************************************************************************
*
* configShow - 
*/
void ProcessListTab::configShow(Starter* starterP)
{
	bool show = true;

	LM_M(("configView: %p", configView));

	if (configView != NULL)
	{
		if (configView->process == starterP->process)
		{
			LM_M(("Hide Config View (process %s)", starterP->process->name));
			show = false;
		}
		
		delete configView;
		configView = NULL;
	}
	else
        LM_M(("configView == NULL - no hiding needed"));

	if (show == true)
	{
		LM_M(("Showing config view for '%s'", starterP->process->name));

		processToBeConfigured        = starterP->process;
		gridForProcessToBeConfigured = rightGrid;
		processConfigRequest(starterP->process);
	}
	else
		LM_M(("Not asking for config view for '%s'", starterP->process->name));
}



/* ****************************************************************************
*
* processConfigRequest - 
*/
void ProcessListTab::processConfigRequest(Process* processP)
{
	int s;

	if ((processP->endpoint != NULL) && (processP->endpoint->state == ss::Endpoint::Connected))
	{
		s = iomMsgSend(processP->endpoint, networkP->endpoint[0], ss::Message::ConfigGet, ss::Message::Msg);

		if (s != 0)
			LM_E(("iomMsgSend error: %d", s));
	}
	else
	{
		if ((networkP->endpoint[2] == NULL) || (networkP->endpoint[2]->state != ss::Endpoint::Connected))
			LM_RVE(("Not connected to controller"));

		LM_M(("Asking samsonController for WorkerConfig for process '%s'", processP->alias));
		s = iomMsgSend(networkP->endpoint[2], networkP->endpoint[0], ss::Message::WorkerConfigGet, ss::Message::Msg, processP->alias, 32);

		if (s != 0)
			LM_RVE(("iomMsgSend error: %d", s));
	}
}
