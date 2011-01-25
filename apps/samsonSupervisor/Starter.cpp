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
#include "traceLevels.h"        // LMT_*

#include "globals.h"            // networkP
#include "actions.h"            // spawnerConnect, spawnerDisconnect
#include "ConfigWindow.h"       // ConfigWindow
#include "Popup.h"              // Popup
#include "Starter.h"            // Own interface



/* ****************************************************************************
*
* Starter::init
*/
void Starter::init(const char* name, Type type)
{
	this->spawner    = NULL;
	this->process    = NULL;
	this->endpoint   = NULL;
	this->checkbox   = NULL;
	this->name       = strdup(name);
	this->type       = type;
}



/* ****************************************************************************
*
* Starter::Starter
*/
Starter::Starter(Process* processP)
{
	char name[128];

	snprintf(name, sizeof(name), "%s@%s", processP->name, processP->host);
	init(name, ProcessStarter);
	process = processP;
}



/* ****************************************************************************
*
* Starter::Starter
*/
Starter::Starter(Spawner* spawnerP)
{
	char name[128];

	snprintf(name, sizeof(name), "%s", spawnerP->host);
	init(name, SpawnerConnecter);
	spawner = spawnerP;
}



/* ****************************************************************************
*
* Starter::typeName
*/
const char* Starter::typeName(void)
{
	switch (type)
	{
	case ProcessStarter:        return "ProcessStarter";
	case SpawnerConnecter:      return "SpawnerConnecter";
	}

	return "UnknownStarterType";
}



/* ****************************************************************************
*
* Starter::check
*/
void Starter::check(void)
{
	if (endpoint == NULL)
		LM_W(("NULL endpoint"));
	else
		LM_M(("endpoint state: '%s'", endpoint->stateName()));

	if ((endpoint != NULL) && (endpoint->state == ss::Endpoint::Connected))
		checkState = Qt::Checked;
	else
		checkState = Qt::Unchecked;


	if (endpoint != NULL)
	{
		LM_T(LMT_CHECK, ("%s %s-Starter '%s' (endpoint %p - '%s' at '%s')",
						 (checkState == Qt::Checked)? "Checking" : "Unchecking",
						 typeName(), name, endpoint, endpoint->name.c_str(), endpoint->ip.c_str()));
	}
	else
	{
		LM_T(LMT_CHECK, ("%s %s-Starter '%s' (NULL endpoint)",
						 (checkState == Qt::Checked)? "Checking" : "Unchecking",
						 typeName(), name));
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
	if (spawner == NULL)
		LM_X(1, ("NULL spawner"));

	LM_M(("IN, checkState: '%s'",  (checkbox->checkState() == Qt::Checked)? "Checked" : "Unchecked"));

	if (checkbox->checkState() == Qt::Checked)
	{
		if (endpoint)
			networkP->endpointRemove(endpoint, "GUI Click - endpoint already existed");

		LM_M(("Connecting to spawner '%s'", spawner->host));
		spawnerConnect(this, spawner);
	}
	else if (checkbox->checkState() == Qt::Unchecked)
	{
		if (endpoint == NULL)
		{
			LM_W(("NULL endpoint - nothing to be done ..."));
			spawner->fd = -1;
			return;
		}

		if (endpoint->state != ss::Endpoint::Connected)
			LM_W(("endpoint pointer ok (%p), but not connected", endpoint));

		LM_M(("removing spawner endpoint"));
		networkP->endpointRemove(endpoint, "GUI Click");
		endpoint    = NULL;
		spawner->fd = -1;
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
		LM_M(("calling processStart - what if process already running ... ?"));
		processStart(process, this);
	}
	else if (checkbox->checkState() == Qt::Unchecked)
	{
		LM_M(("calling processKill"));
		processKill(process, this);
	}
}



/* ****************************************************************************
*
* Starter::configureClicked
*/
void Starter::configureClicked(void)
{
	if (endpoint == NULL)
	{
		char info[128];

		snprintf(info, sizeof(info), "%s is not connected", name);
		new Popup("Error", info);
	}
	else
		new ConfigWindow(endpoint);
}
