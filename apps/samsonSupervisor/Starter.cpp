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

#include "actions.h"            // spawnerConnect, spawnerDisconnect
#include "Starter.h"            // Own interface



/* ****************************************************************************
*
* Starter::Starter
*/
Starter::Starter(const char* type, char* name)
{
	checkbox = new QCheckBox(QString(name), this);

	if (strcmp(type, "Spawner") == 0)
		connect(checkbox, SIGNAL(clicked()), this, SLOT(spawnerClicked()));
	else if (strcmp(type, "Process") == 0)
		connect(checkbox, SIGNAL(clicked()), this, SLOT(processClicked()));
	else
		LM_X(1, ("bad type '%s' for Starter", type));

	checkState = Qt::Unchecked;
	checkbox->setCheckState(checkState);

	spawner    = NULL;
	process    = NULL;
	endpoint   = NULL;
	connected  = false;

	this->name = strdup(name);
	this->type = strdup(type);

	checkbox->show();
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
			spawnerDisconnect(spawner);
			connected = false;
		}
	}
}



/* ****************************************************************************
*
* Starter::spawnerSet
*/
void Starter::spawnerSet(Spawner* s)
{
	spawner = s;
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
			processStart(process);
			connected = true;
		}
	}
	else if (checkbox->checkState() == Qt::Unchecked)
	{
		if (connected == false)
			LM_W(("process '%s' in '%s' not running", process->name, process->host));
		else
		{
			processKill(process);
			connected = false;
		}
	}
}



/* ****************************************************************************
*
* Starter::processSet
*/
void Starter::processSet(Process* s)
{
	process = s;
}




