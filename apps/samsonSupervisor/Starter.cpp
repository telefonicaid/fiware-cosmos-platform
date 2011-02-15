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
	box->addWidget(logButton);
	box->addWidget(nameButton);
	box->addStretch(500);

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
		logButton->setDisabled(false);

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
		logButton->setDisabled(true);

		if (process->type != PtSpawner)
        {
			startButton->setIcon(redIcon);
			startButton->setToolTip("Start Process");
		}

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



extern ss::Message::Worker* workerLookup(const char* alias);
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
	int                     s;
	char*                   alias = (char*) "no_alias";
	char*                   end;

	strcpy(spawnData.name, process->name);
	spawnData.argCount = 0;

	if (strcmp(process->name, "Controller") == 0)
	{
		char workersV[16];

		LM_M(("Starting Controller in host %s", process->host));

		spawnData.argCount = 2;

		memset(spawnData.args, sizeof(spawnData.args), 0);
		end = spawnData.args;

		strcpy(end, "-workers");
		end += strlen(end);
		++end;

		LM_TODO(("Include number of workers in config view for controller ..."));
		LM_TODO(("workers is a command line option for samsonSupervisor for now ..."));
		extern int workers;
		snprintf(workersV, sizeof(workersV), "%d", workers);

		strcpy(end, workersV);
		end += strlen(end);
		++end;

		*end = 0;
	}
	else
	{
		ss::Message::Worker* workerP;

		workerP = workerLookup(process->alias);
		if (workerP == NULL)
			LM_X(1, ("Cannot find worker '%s' (%s@%s)", process->alias, process->name, process->host));

		LM_T(LmtProcessStart, ("starting process '%s' in '%s' with no parameters", process->name, process->host));
		processListShow("starting process");

		spawnData.argCount = 4;

		memset(spawnData.args, sizeof(spawnData.args), 0);
		end = spawnData.args;

		strcpy(end, "-alias");
		end += strlen(end);
		++end;

		strcpy(end, process->alias);
		end += strlen(end);
		++end;

		strcpy(end, "-controller");
		end += strlen(end);
		++end;

		strcpy(end, networkP->endpoint[2]->ip.c_str());
		end += strlen(end);
		++end;

		*end = 0;

		for (int ix = 0; ix < 64; ix += 8)
			LM_M(("%02x %02x %02x %02x %02x %02x %02x %02x",
				  spawnData.args[ix + 0] & 0xFF, 
				  spawnData.args[ix + 1] & 0xFF, 
				  spawnData.args[ix + 2] & 0xFF, 
				  spawnData.args[ix + 3] & 0xFF, 
				  spawnData.args[ix + 4] & 0xFF, 
				  spawnData.args[ix + 5] & 0xFF, 
				  spawnData.args[ix + 6] & 0xFF, 
				  spawnData.args[ix + 7] & 0xFF));
	}

	LM_T(LmtProcessStart, ("starting %s (alias '%s') via spawner %p (host: '%s', fd: %d).",
						   spawnData.name,
						   process->alias,
						   process->spawnInfo->spawnerP,
						   process->spawnInfo->spawnerP->host,
						   process->spawnInfo->spawnerP->endpoint->rFd));

	if (process->spawnInfo == NULL)
		LM_X(1, ("NULL spawnInfo for process '%s@%d'", process->name, process->host));
	if (process->spawnInfo->spawnerP == NULL)
		LM_X(1, ("NULL spawner pointer for process '%s@%d'", process->name, process->host));
	if (process->spawnInfo->spawnerP->endpoint == NULL)
		LM_X(1, ("NULL spawner pointer for process '%s@%d'", process->name, process->host));

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
