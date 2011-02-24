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
#include "Process.h"            // ss::Process
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
* imageV - stores absolute path to images
*/
static char imageV[256];



/* ****************************************************************************
*
* Starter::Starter
*/
Starter::Starter(ss::Process* processP)
{
	process  = processP;
}



/* ****************************************************************************
*
* Starter::qtInit - 
*/
void Starter::qtInit(QVBoxLayout* spawnerLayout, QVBoxLayout* workerLayout, QVBoxLayout* controllerLayout)
{
	QIcon        redIcon(imagePath("red-ball.gif", imageV, sizeof(imageV)));
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

	if (process->type != ss::PtSpawner)	
		startButton->connect(startButton, SIGNAL(clicked()), this, SLOT(startClicked()));

	box->addWidget(startButton);
	box->addWidget(logButton);
	box->addWidget(nameButton);
	box->addStretch(500);

	logButton->connect(logButton,         SIGNAL(clicked()), this, SLOT(logClicked()));
	nameButton->connect(nameButton,       SIGNAL(clicked()), this, SLOT(nameClicked()));
	
	if (process->type == ss::PtSpawner)
		spawnerLayout->addLayout(box);
	else if (strcmp(process->name, "Worker") == 0)
		workerLayout->addLayout(box);
	else
		controllerLayout->addLayout(box);

	LM_T(LmtStarter, ("looking up endpoint for a starter"));
	if (process->endpoint == NULL)
	{
		if (process->type == ss::PtSpawner)
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
#if 1
	static QIcon  greenIcon(imagePath("green-ball.gif", imageV, sizeof(imageV)));
	static QIcon  redIcon(imagePath("red-ball.gif", imageV, sizeof(imageV)));
#else
	I get a SIGSEGV here if I dont use static QIcons ...

#0  0xb76a08b8 in QIcon::pixmap(QSize const&, QIcon::Mode, QIcon::State) const () from /usr/lib/libQtGui.so.4
#1  0xb7993f30 in QGtkStyle::drawControl(QStyle::ControlElement, QStyleOption const*, QPainter*, QWidget const*) const () from /usr/lib/libQtGui.so.4
#2  0xb7993dd7 in QGtkStyle::drawControl(QStyle::ControlElement, QStyleOption const*, QPainter*, QWidget const*) const () from /usr/lib/libQtGui.so.4
#3  0xb7a9231b in QPushButton::paintEvent(QPaintEvent*) () from /usr/lib/libQtGui.so.4

etc ...

	QIcon  greenIcon(imagePath("green-ball.gif", imageV, sizeof(imageV)));
	QIcon  redIcon(imagePath("red-ball.gif", imageV, sizeof(imageV)));
#endif

	LM_T(LmtStarter, ("CHECKING (%s) Starter for %s@%s", reason, process->name, process->host));

	if (process->endpoint != NULL)
		LM_T(LmtCheck, ("endpoint state: '%s'", process->endpoint->stateName()));

	if ((process->endpoint != NULL) && (process->endpoint->state == ss::Endpoint::Connected))
	{
		logButton->setDisabled(false);

		if (process->type != ss::PtSpawner)
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
		startButton->setIcon(redIcon);

		if (process->type == ss::PtSpawner)
			startButton->setToolTip("Start Process");
		else
			startButton->setToolTip("Connect to Spawner");

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

	if (process->type == ss::PtSpawner)
	{
		if ((process->endpoint == NULL) || (process->endpoint->state != ss::Endpoint::Connected))
			processConnect();
		else
		{
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
		QIcon redIcon(imagePath("red-ball.gif", imageV, sizeof(imageV)));

		logButton->setIcon(redIcon);
		logButton->setToolTip("Make process send log information");		
		process->sendsLogs = false;
		iomMsgSend(process->endpoint, networkP->endpoint[0], ss::Message::LogSendingOff, ss::Message::Msg);
	}
	else
	{
		QIcon greenIcon(imagePath("green-ball.gif", imageV, sizeof(imageV)));

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
	int s;
	
	if (process == NULL)
	   LM_RVE(("NULL process"));

	if (process->spawnerP == NULL)
		LM_X(1, ("NULL spawner pointer for process '%s@%d'", process->name, process->host));
	if (process->spawnerP->endpoint == NULL)
		LM_X(1, ("NULL spawner pointer for process '%s@%d'", process->name, process->host));

	s = iomMsgSend(process->spawnerP->endpoint->wFd, process->spawnerP->host, "samsonSupervisor", ss::Message::ProcessSpawn, ss::Message::Msg, process, sizeof(*process));
    if (s != 0)
		LM_RVE(("iomMsgSend: error %d", s));

	LM_T(LmtProcessStart, ("started process '%s' in '%s')", process->name, process->host));
	LM_T(LmtProcessStart, ("Connecting to newly started process (%s) ...", process->name));

	char*  alias = (char*) process->alias;

	int tries = 0;
	while (1)
	{
		int fd;

		if (strcmp(process->name, "Controller") == 0)
		{
			fd = iomConnect(process->spawnerP->host, CONTROLLER_PORT);
			if (fd != -1)
			{
				process->endpoint = networkP->endpointAdd("connected to spawner", fd, fd, "Controller", "Controller", 0, ss::Endpoint::Controller, process->spawnerP->host, CONTROLLER_PORT);
				break;
			}			
		}
		else if (strcmp(process->name, "Worker") == 0)
		{
			fd = iomConnect(process->spawnerP->host, WORKER_PORT);

			if (fd != -1)
			{
				process->endpoint = networkP->endpointAdd("connected to worker", fd, fd, process->name, alias, 0, ss::Endpoint::Temporal, process->host, WORKER_PORT);
				break;
			}

			LM_TODO(("This endpoint is TEMPORAL and will be changed when the Hello is received - fix this problem!"));
		}
		else
			LM_X(1, ("Bad process name: '%s'", process->name));

		if (++tries > 20)
		{
			char errorText[256];

			if (strcmp(process->name, "Worker") == 0)
				snprintf(errorText, sizeof(errorText), "Error connecting to Samson Worker in '%s', port %d", process->spawnerP->host, WORKER_PORT);
			else if (strcmp(process->name, "Controller") == 0)
				snprintf(errorText, sizeof(errorText), "Error connecting to Samson Controller in '%s', port %d", process->spawnerP->host, CONTROLLER_PORT);
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
		LM_W(("Sending 'Die' to '%s' at '%s' (name: '%s')", process->endpoint->typeName(), process->endpoint->ip, process->endpoint->name.c_str()));
		s = iomMsgSend(process->endpoint->wFd, process->endpoint->ip, "samsonSupervisor", ss::Message::Die, ss::Message::Msg);
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
