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
#include <QToolButton>
#include <QPushButton>

#include "logMsg.h"             // LM_*
#include "traceLevels.h"        // Trace Levels
#include "iomMsgSend.h"         // iomMsgSend

#include "globals.h"            // networkP
#include "Popup.h"              // Popup
#include "actions.h"            // spawnerConnect, spawnerDisconnect
#include "processList.h"        // processTypeName, ...
#include "Starter.h"            // Own interface



/* ****************************************************************************
*
* Starter::Starter
*/
Starter::Starter(Process* processP)
{
	process  = processP;
}



/* ****************************************************************************
*
* Starter::qtInit - 
*/
void Starter::qtInit(QVBoxLayout* layout)
{
	QIcon        redIcon("images/red-ball.gif");
	QHBoxLayout* box = new QHBoxLayout();

	LM_T(LmtStarter, ("Creating checkbox for '%s'", process->name));

	connectButton  = new QPushButton();
	connectButton->setIcon(redIcon);
	connectButton->setToolTip("Connect");
	connectButton->setFlat(true);

	logButton = new QPushButton();
	logButton->setIcon(redIcon);
	logButton->setToolTip("Make process send log information");
	logButton->setFlat(true);

	nameButton = new QPushButton(QString(process->name) + "@" + process->host);
	nameButton->setToolTip("Configure process");
	nameButton->setFlat(true);

	startButton = new QPushButton();
	startButton->setIcon(redIcon);
	startButton->setToolTip("Start Process");
	startButton->setFlat(true);

	if (process->type != PtSpawner)	
		startButton->connect(startButton, SIGNAL(clicked()), this, SLOT(startClicked()));

	box->addWidget(startButton);
	box->addWidget(connectButton);
	box->addWidget(logButton);
	box->addWidget(nameButton);
	box->addStretch(500);

	connectButton->connect(connectButton, SIGNAL(clicked()), this, SLOT(connectClicked()));
	logButton->connect(logButton,         SIGNAL(clicked()), this, SLOT(logClicked()));
	nameButton->connect(nameButton,       SIGNAL(clicked()), this, SLOT(nameClicked()));
	
	layout->addLayout(box);

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

	check("qtInit");
}



/* ****************************************************************************
*
* Starter::check
*/
void Starter::check(const char* reason)
{
	QIcon  greenIcon("images/green-ball.gif");
	QIcon  redIcon("images/red-ball.gif");

	LM_T(LmtStarter, ("CHECKING (%s) Starter for %s@%s", reason, process->name, process->host));

	if (process->endpoint == NULL)
		LM_W(("NULL endpoint"));
	else
		LM_T(LmtCheck, ("endpoint state: '%s'", process->endpoint->stateName()));

	if ((process->endpoint != NULL) && (process->endpoint->state == ss::Endpoint::Connected))
	{
		connectButton->setIcon(greenIcon);
		connectButton->setToolTip("Disconnect");

		if (process->type != PtSpawner)
		{
			startButton->setIcon(greenIcon);
			startButton->setToolTip("Kill process");
		}
		else
		{
		   startButton->setIcon(greenIcon);
		   startButton->setToolTip("Nothing ...");		   
		}

		connectButton->setDisabled(false);
		logButton->setDisabled(false);

		if (process->sendsLogs)
		{
			logButton->setIcon(greenIcon);
			logButton->setToolTip("Make process stop sending log information");		
		}
		else
		{
			logButton->setIcon(redIcon);
			logButton->setToolTip("Make process send log information");		
		}
	}
	else
	{
		if (process->type != PtSpawner)
        {
			startButton->setIcon(redIcon);
			startButton->setToolTip("Start Process");
		}

		logButton->setDisabled(true);
		connectButton->setDisabled(true);
	}
}



/* ****************************************************************************
*
* startClicked - 
*/
void Starter::startClicked(void)
{
	if (process == NULL)
		LM_X(1, ("NULL process"));

	if (process->type == PtSpawner)
	{
		if ((process->endpoint == NULL) || (process->endpoint->state != ss::Endpoint::Connected))
			processConnect(process);
		else
		{
			networkP->endpointRemove(process->endpoint, "GUI Click");
			process->endpoint    = NULL;
		}

		return;
	}

	if ((process->endpoint == NULL) || (process->endpoint->state != ss::Endpoint::Connected))
	{
		LM_T(LmtProcessStart, ("calling processStart - what if process already running ... ?"));
		processStart(process, this);
	}
	else
	{
		LM_T(LmtProcessKill, ("calling processKill"));
		processKill(process, this);
	}

	check("startClicked");
}



/* ****************************************************************************
*
* connectClicked - 
*/
void Starter::connectClicked(void)
{
	new Popup("ToDo",
			  "Remove CONNECT/DISCONNECT column of 'balls'.\n"
			  "START and CONNECT to use the same button.\n"
			  "  Start for Controller/Worker and\n"
			  "  Connect for Spawner.\n\n"
			  "Log button must stay, of course."); 
}



/* ****************************************************************************
*
* logClicked - 
*/
void Starter::logClicked(void)
{
	if ((process->endpoint == NULL) || (process->endpoint->state != ss::Endpoint::Connected))
	{
		new Popup("Not running", "Cannot make a non-running process send log information ...");
		return;
	}

	if (process->sendsLogs)
	{
		QIcon redIcon("images/red-ball.gif");

		logButton->setIcon(redIcon);
		logButton->setToolTip("Make process send log information");		
		process->sendsLogs = false;
		iomMsgSend(process->endpoint, networkP->endpoint[0], ss::Message::LogSendingOff, ss::Message::Msg);
	}
	else
	{
		QIcon greenIcon("images/green-ball.gif");

		logButton->setIcon(greenIcon);
		logButton->setToolTip("Make process stop sending log information");		
		process->sendsLogs = true;
		iomMsgSend(process->endpoint, networkP->endpoint[0], ss::Message::LogSendingOn, ss::Message::Msg);
	}
}



/* ****************************************************************************
*
* nameClicked - 
*/
void Starter::nameClicked(void)
{
	if ((process->endpoint != NULL) && (process->endpoint->state == ss::Endpoint::Connected))
		tabManager->processListTab->configShow(this);
	else
	{
		new Popup("Not Implemented", "As this process isn't running,\n"
				  "show to the right of Processes Tab all possible command line options\nto start the process.");
	}
}
