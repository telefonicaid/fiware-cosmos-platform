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
* Starter::spawnerClicked
*/
void Starter::spawnerClicked(void)
{
	if (spawner == NULL)
		LM_X(1, ("NULL spawner"));

	if (checkbox->checkState() == Qt::Checked)
	{
		if (connected == true)
			LM_W(("Already connected to spawner at '%s'", spawner->host));
		else
		{
			spawnerConnect(spawner);
			connected = true;
		}
	}
	else if (checkbox->checkState() == Qt::Unchecked)
	{
		if (connected == false)
			LM_W(("Not connected to spawner at '%s'", spawner->host));
		else
		{
			if (endpoint == NULL)
				LM_W(("NULL endpoint"));
			else if (endpoint->state != ss::Endpoint::Connected)
				LM_W(("Not connected to endpoint"));
			else
				networkP->endpointRemove(endpoint, "GUI Click");

			connected = false;
		}
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
		if (connected == true)
			LM_W(("Already started process '%s' in '%s'", process->name, process->host));
		else
		{
			processStart(process, this);
			connected = true;
		}
	}
	else if (checkbox->checkState() == Qt::Unchecked)
	{
		if (connected == false)
			LM_W(("process '%s' in '%s' not running", process->name, process->host));
		else
		{
			processKill(process, this);
			connected = false;
		}
	}
}



/* ****************************************************************************
*
* Starter::forceCheck
*/
void Starter::forceCheck(void)
{
	if (endpoint != NULL)
		// LM_T(LMT_CHECK, ("Checking %s-Starter '%s' (endpoint %p - '%s' at '%s')", typeName(), name, endpoint, endpoint->name.c_str(), endpoint->ip.c_str()));
		LM_M(("Checking %s-Starter '%s' (endpoint %p - '%s' at '%s')", typeName(), name, endpoint, endpoint->name.c_str(), endpoint->ip.c_str()));
	else
		// LM_T(LMT_CHECK, ("Checking %s-Starter '%s' (NULL endpoint)", typeName(), name));
		LM_M(("Checking %s-Starter '%s' (NULL endpoint)", typeName(), name));

	checkState = Qt::Checked;
	connected  = true;

	if (checkbox != NULL)
		checkbox->setCheckState(checkState);
	else
		LM_W(("NULL checkbox"));
}



/* ****************************************************************************
*
* Starter::forceUncheck
*/
void Starter::forceUncheck(void)
{
	if (endpoint != NULL)
		// LM_T(LMT_CHECK, ("Unchecking %s-Starter '%s' (endpoint %p - '%s' at '%s')", typeName(), name, endpoint, endpoint->name.c_str(), endpoint->ip.c_str()));
		LM_M(("Unchecking %s-Starter '%s' (endpoint %p - '%s' at '%s')", typeName(), name, endpoint, endpoint->name.c_str(), endpoint->ip.c_str()));
	else
		// LM_T(LMT_CHECK, ("Unchecking %s-Starter '%s' (NULL endpoint)", typeName(), name));
		LM_M(("Unchecking %s-Starter '%s' (NULL endpoint)", typeName(), name));

	checkState = Qt::Unchecked;
	connected  = false;

	if (checkbox != NULL)
		checkbox->setCheckState(checkState);
	else
		LM_W(("NULL checkbox"));
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
		forceCheck();
	else
		forceUncheck();
}



/* ****************************************************************************
*
* Starter::checked
*/
bool Starter::checked(void)
{
	return connected;
}
