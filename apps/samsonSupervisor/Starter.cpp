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
#include "Message.h"            // ss::Message
#include "iomMsgSend.h"         // iomMsgSend
#include "iomConnect.h"         // iomConnect
#include "ports.h"              // SPAWNER_PORT, ...

#include "globals.h"            // networkP
#include "Popup.h"              // Popup
#include "configFile.h"         // configFileParse
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
void Starter::qtInit(QVBoxLayout* spawnerLayout, QVBoxLayout* workerLayout, QVBoxLayout* controllerLayout)
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

	if (strcmp(process->name, "Worker") == 0)
		nameButton = new QPushButton(QString(process->name) + "@" + process->host + " (" + process->alias + ")");
	else
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
	// box->addWidget(connectButton);
	box->addWidget(logButton);
	box->addWidget(nameButton);
	box->addStretch(500);

	// connectButton->connect(connectButton, SIGNAL(clicked()), this, SLOT(connectClicked()));
	logButton->connect(logButton,         SIGNAL(clicked()), this, SLOT(logClicked()));
	nameButton->connect(nameButton,       SIGNAL(clicked()), this, SLOT(nameClicked()));
	
	if (process->type == PtSpawner)
		spawnerLayout->addLayout(box);
	else if (strcmp(process->name, "Worker") == 0)
		workerLayout->addLayout(box);
	else
		controllerLayout->addLayout(box);

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

		if ((process->host == NULL) || (process->host[0] == 0) || (strcmp(process->host, "ip") == 0))
			startButton->setDisabled(true);
		else
			startButton->setDisabled(false);
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
		{
			LM_M(("Connecting to spawner in %s", process->host));
			processConnect();
		}
		else
		{
			LM_M(("Disconnecting from spawner in %s (by removing its endpoint)", process->host));
			LM_TODO(("Send IDie first ?"));
			networkP->endpointRemove(process->endpoint, "GUI Click");
			process->endpoint    = NULL;
		}

		return;
	}

	if ((process->endpoint == NULL) || (process->endpoint->state != ss::Endpoint::Connected))
	{
		LM_T(LmtProcessStart, ("calling processStart - what if process already running ... ?"));
		processStart();
	}
	else
	{
		LM_T(LmtProcessKill, ("calling processKill"));
		processKill();
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
	tabManager->processListTab->configShow(this);
}



/* ****************************************************************************
*
* processStart - start a process
*/
void Starter::processStart(void)
{
	ss::Message::SpawnData  spawnData;
	int                     ix;
	char*                   end;
	int                     s;
	char*                   alias = (char*) "no_alias";

    int    args = 0;
    char*  argVec[20];
	
	LM_TODO(("Here I send a message to controller to retrieve the command line options for this process"));
	LM_TODO(("'etc/platformProcesses' will no longer be used"));
	if (configFileParse(process->host, process->name, &args, argVec) == -1)
	{
		char eText[256];

		snprintf(eText,
				 sizeof(eText),
				 "Unable to connect to worker in '%s'.\nAlso unable to find info on Worker in config file.\nUnable to connect to Worker, sorry.",
				 process->host);
		new Popup("Cannot connect to Worker", eText, true);
		return;
	}

	LM_T(LmtProcessStart, ("starting process '%s' in '%s' with %d parameters", process->name, process->host, args));
	processListShow("starting process");

	spawnData.argCount = args;
	strcpy(spawnData.name, process->name);
	memset(spawnData.args, sizeof(spawnData.args), 0);

	end = spawnData.args;

	for (ix = 0; ix < args; ix++)
	{
		strcpy(end, argVec[ix]);
		LM_T(LmtProcessStart, ("parameter %d: '%s'", ix, end));
		end += strlen(argVec[ix]) + 1; // leave one ZERO character
		if (strcmp(argVec[ix], "-alias") == 0)
			alias = end;
	}
	*end = 0;

	LM_T(LmtProcessStart, ("starting %s via spawner %p (host: '%s', fd: %d). %d params",
						   spawnData.name,
						   process->spawnInfo->spawnerP,
						   process->spawnInfo->spawnerP->host,
						   process->spawnInfo->spawnerP->endpoint->rFd,
						   spawnData.argCount));

	if (strcmp(spawnData.name, "Controller") == 0)
		s = iomMsgSend(process->spawnInfo->spawnerP->endpoint->wFd, process->spawnInfo->spawnerP->host, "samsonSupervisor", ss::Message::ControllerSpawn, ss::Message::Msg, &spawnData, sizeof(spawnData));
	else if (strcmp(spawnData.name, "Worker") == 0)
		s = iomMsgSend(process->spawnInfo->spawnerP->endpoint->wFd, process->spawnInfo->spawnerP->host, "samsonSupervisor", ss::Message::WorkerSpawn, ss::Message::Msg, &spawnData, sizeof(spawnData));
	if (s != 0)
		LM_E(("iomMsgSend: error %d", s));

	LM_T(LmtProcessStart, ("started process '%s' in '%s')", process->name, process->host));
	LM_T(LmtProcessStart, ("Connecting to newly started process (%s) ...", spawnData.name));

	int tries = 0;
	while (1)
	{
		int fd;

		if (strcmp(spawnData.name, "Controller") == 0)
		{
			fd = iomConnect(process->spawnInfo->spawnerP->host, CONTROLLER_PORT);
			if (fd != -1)
			{
				process->endpoint = networkP->endpointAdd("connected to spawner", fd, fd, "Controller", "Controller", 0, ss::Endpoint::Controller, process->spawnInfo->spawnerP->host, CONTROLLER_PORT);
				break;
			}			
		}
		else if (strcmp(spawnData.name, "Worker") == 0)
		{
			fd = iomConnect(process->spawnInfo->spawnerP->host, WORKER_PORT);

			if (fd != -1)
			{
				process->endpoint = networkP->endpointAdd("connected to worker", fd, fd, spawnData.name, alias, 0, ss::Endpoint::Temporal, process->host, WORKER_PORT);
				break;
			}

			LM_TODO(("This endpoint is TEMPORAL and will be changed when the Hello is received - fix this problem!"));
		}
		
		if (++tries > 20)
		{
			char errorText[256];

			if (strcmp(spawnData.name, "Worker") == 0)
				snprintf(errorText, sizeof(errorText), "Error connecting to Samson Worker in '%s', port %d", process->spawnInfo->spawnerP->host, WORKER_PORT);
			else if (strcmp(spawnData.name, "Controller") == 0)
				snprintf(errorText, sizeof(errorText), "Error connecting to Samson Controller in '%s', port %d", process->spawnInfo->spawnerP->host, CONTROLLER_PORT);
			else
				snprintf(errorText, sizeof(errorText), "Error connecting ");

			new Popup("Connect Error", errorText);
			check("processStart error");
			return;
		}

		usleep(50000);
	}

	check("processStart");
}



/* ****************************************************************************
*
* processKill - kill a process
*/
void Starter::processKill(void)
{
	int       s;
	
	LM_T(LmtProcessKill, ("killing process '%s' in host '%s'", process->name, process->host));
	if (process->endpoint == NULL)
		LM_E(("can't kill starter for process '%s' at %s as its endpoint is NULL", process->name, process->host));
	else if (process->endpoint->state != ss::Endpoint::Connected)
		LM_E(("can't kill starter for process '%s' at %s as its endpoint is in state '%s'", process->name, process->host, process->endpoint->stateName()));
	else
	{
		LM_T(LmtDie, ("Now really sending 'Die' to '%s' at '%s' (name: '%s')", process->endpoint->typeName(), process->endpoint->ip.c_str(), process->endpoint->name.c_str()));
		s = iomMsgSend(process->endpoint->wFd, process->endpoint->ip.c_str(), "samsonSupervisor", ss::Message::Die, ss::Message::Msg);
	}
}



/* ****************************************************************************
*
* processConnect - connect to process
*/
void Starter::processConnect(void)
{
	int fd;

	LM_T(LmtProcessConnect, ("connecting to process in %s on port %d", process->host, process->port));

	fd = iomConnect(process->host, process->port);
	if (fd == -1)
	{
		char errorText[256];

		snprintf(errorText, sizeof(errorText), "error connecting to process in host '%s', port %d", process->host, process->port);
		new Popup("Connect Error", errorText);
		
		return;
	}

	LM_T(LmtProcessConnect, ("Calling endpointAdd for process '%s'", process->host));
	process->endpoint = networkP->endpointAdd("connected to process",
										 fd,
										 fd,
										 "Process",
										 NULL,
										 0,
										 ss::Endpoint::Temporal,
										 std::string(process->host),
										 process->port);
}
