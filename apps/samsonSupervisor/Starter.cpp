/* ****************************************************************************
*
* FILE                     Starter.cpp
*
* AUTHOR                   Ken Zangelin
*
* CREATION DATE            Jan 13 2011
*
*/
#include <QObject>
#include <QCheckBox>

#include "logMsg.h"             // LM_*
#include "traceLevels.h"        // Trace Levels

#include "globals.h"            // networkP
#include "actions.h"            // spawnerConnect, spawnerDisconnect
#include "ConfigWindow.h"       // ConfigWindow
#include "Popup.h"              // Popup
#include "processList.h"        // processTypeName, ...
#include "Starter.h"            // Own interface



/* ****************************************************************************
*
* Starter::Starter
*/
Starter::Starter(Process* processP)
{
	process  = processP;
	checkbox = NULL;
}


/* ****************************************************************************
*
* Starter::qtInit - 
*/
void Starter::qtInit(QGridLayout* grid, int row, int column)
{
	LM_T(LmtStarter, ("Creating checkbox for '%s'", process->name));

	checkbox     = new QCheckBox(QString(process->name), this);
	configButton = new QPushButton("Configure");

	grid->addWidget(checkbox,     row + 1, column);
	grid->addWidget(configButton, row + 1, column + 1);

	if (process->type == PtSpawner)
		checkbox->connect(checkbox, SIGNAL(clicked()), this, SLOT(spawnerClicked()));
	else
		checkbox->connect(checkbox, SIGNAL(clicked()), this, SLOT(processClicked()));

	configButton->connect(configButton, SIGNAL(clicked()), this, SLOT(configureClicked()));

	LM_T(LmtStarter, ("looking up endpoint for a starter"));
	if (process->endpoint == NULL)
	{
		if (process->type == PtSpawner)
			process->endpoint = networkP->endpointLookup(ss::Endpoint::Spawner, process->host);
		else if (strcmp(process->name, "Controller") == 0)
			process->endpoint = networkP->endpointLookup(ss::Endpoint::Controller, process->host);
		else if (strcmp(process->name, "Worker") == 0)
			process->endpoint = networkP->endpointLookup(ss::Endpoint::Worker, process->host);
	}

	check();
}



/* ****************************************************************************
*
* Starter::check
*/
void Starter::check(void)
{
	if (process->endpoint == NULL)
		LM_W(("NULL endpoint"));
	else
	   LM_T(LmtCheck, ("endpoint state: '%s'", process->endpoint->stateName()));

	if ((process->endpoint != NULL) && (process->endpoint->state == ss::Endpoint::Connected))
		checkState = Qt::Checked;
	else
		checkState = Qt::Unchecked;


	if (process->endpoint != NULL)
	{
		LM_T(LmtCheck, ("%s %s-Starter '%s' (endpoint %p - '%s' at '%s')",
						(checkState == Qt::Checked)? "Checking" : "Unchecking",
						processTypeName(process), process->name, process->endpoint, process->endpoint->name.c_str(), process->endpoint->ip.c_str()));
	}
	else
	{
		LM_T(LmtCheck, ("%s %s-Starter '%s' (NULL endpoint)",
						 (checkState == Qt::Checked)? "Checking" : "Unchecking",
						 processTypeName(process), process->name));
	}

	if (checkbox != NULL)
		checkbox->setCheckState(checkState);
	else
		LM_W(("NULL checkbox"));
}



/* ****************************************************************************
*
* Starter::spawnerClicked
*/
void Starter::spawnerClicked(void)
{
	LM_T(LmtCheck, ("IN, checkState: '%s'",  (checkbox->checkState() == Qt::Checked)? "Checked" : "Unchecked"));

	if (checkbox->checkState() == Qt::Checked)
	{
		if (process->endpoint)
			networkP->endpointRemove(process->endpoint, "GUI Click - endpoint already existed");

		LM_T(LmtSpawnerConnect, ("Connecting to spawner '%s'", process->host));
		processConnect(process);
	}
	else if (checkbox->checkState() == Qt::Unchecked)
	{
		if (process->endpoint == NULL)
		{
			LM_W(("NULL endpoint - nothing to be done ..."));
			return;
		}

		if (process->endpoint->state != ss::Endpoint::Connected)
			LM_W(("endpoint pointer ok (%p), but not connected", process->endpoint));

		networkP->endpointRemove(process->endpoint, "GUI Click");
		process->endpoint    = NULL;
	}
}



/* ****************************************************************************
*
* Starter::processClicked
*/
void Starter::processClicked(void)
{
	if (process == NULL)
		LM_X(1, ("NULL process"));

	if (checkbox->checkState() == Qt::Checked)
	{
		LM_T(LmtProcessStart, ("calling processStart - what if process already running ... ?"));
		processStart(process, this);
	}
	else if (checkbox->checkState() == Qt::Unchecked)
	{
		LM_T(LmtProcessKill, ("calling processKill"));
		processKill(process, this);
	}
}



/* ****************************************************************************
*
* Starter::configureClicked
*/
void Starter::configureClicked(void)
{
	if (process->endpoint == NULL)
	{
		char info[128];

		snprintf(info, sizeof(info), "%s is not connected", process->name);
		new Popup("Error", info);
	}
	else
		new ConfigWindow(process->endpoint);
}
