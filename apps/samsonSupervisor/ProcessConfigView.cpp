/* ****************************************************************************
*
* FILE                     ProcessConfigView.cpp
*
* AUTHOR                   Ken Zangelin
*
* CREATION DATE            Feb 09 2011
*
*/
#include <QGridLayout>
#include <QLabel>
#include <QSize>
#include <QCheckBox>
#include <QPushButton>
#include <QListWidget>

#include "logMsg.h"             // LM_*
#include "traceLevels.h"        // Trace Levels

#include "globals.h"            // networkP, ...
#include "Popup.h"              // Popup
#include "ports.h"              // SPAWNER_PORT
#include "iomConnect.h"         // iomConnect
#include "InfoWin.h"            // InfoWin
#include "iomMsgSend.h"         // iomMsgSend
#include "iomMsgAwait.h"        // iomMsgAwait
#include "iomMsgRead.h"         // iomMsgRead
#include "Endpoint.h"           // Endpoint
#include "Process.h"            // Process
#include "processList.h"        // spawnerLookup
#include "starterList.h"        // starterAdd
#include "ProcessConfigView.h"  // Own interface



/* ****************************************************************************
*
* ProcessConfigView::~ProcessConfigView - 
*/
ProcessConfigView::~ProcessConfigView()
{
	unsigned int  ix;
	QWidget*      widgetV[] =
	{
		label,
		sButton,
		traceLevelLabel,
		traceLevelList,
		verboseBox,
		debugBox,
		readsBox,
		writesBox,
		toDoBox,
		allTraceLevelsItem,
		hostLabel,
		hostEdit
	};

	for (ix = 0; ix < TRACE_LEVELS; ix++)
		delete traceLevelItem[ix];

	for (ix = 0; ix < sizeof(widgetV) / sizeof(widgetV[0]); ix++)
	{
		if (widgetV[ix] == NULL)
			continue;

		widgetV[ix]->hide();
		grid->removeWidget(widgetV[ix]);
		delete widgetV[ix];
	}

	if (ahLayout != NULL)
	{
		grid->removeItem(ahLayout);
		delete ahLayout;
	}
}



/* ****************************************************************************
*
* init - 
*/
void ProcessConfigView::init(QGridLayout* grid, ss::Process* process)
{
	ss::Endpoint*  endpoint = process->endpoint;
	char           processName[256];
	QFont          labelFont("Times", 18, QFont::Normal);
	QFont          traceFont("Helvetica", 10, QFont::Normal);

	this->grid         = grid;
	this->process      = process;
	this->hostEditable = false;

	ahLayout   = NULL;
	hostLabel  = NULL;
	hostEdit   = NULL;

	if ((process->alias != NULL) && (process->alias[0] != 0))
	{
		if ((process->host == NULL) || (process->host[0] == 0) || (strcmp(process->host, "ip") == 0))
		{
			snprintf(processName, sizeof(processName), "%s", process->alias);
			hostEditable = true;
		}
		else
			snprintf(processName, sizeof(processName), "%s (%s@%s)", process->alias, process->name, process->host);
	}
	else
		snprintf(processName, sizeof(processName), "%s@%s", process->name, process->host);
	
	label              = new QLabel(processName);

	verboseBox         = new QCheckBox("Verbose");
	debugBox           = new QCheckBox("Debug");
	readsBox           = new QCheckBox("Reads");
	writesBox          = new QCheckBox("Writes");
	toDoBox            = new QCheckBox("ToDo");

	if ((endpoint != NULL) && (endpoint->state == ss::Endpoint::Connected))
		sButton        = new QPushButton("Send");
	else
		sButton        = new QPushButton("Save");

	traceLevelLabel    = new QLabel("Trace Levels");
	traceLevelList     = new QListWidget();
	allTraceLevelsItem = new QPushButton("ALL/None");

	label->setFont(labelFont);
	traceLevelLabel->setFont(traceFont);
	traceLevelList->setFont(traceFont);

	if ((endpoint != NULL) && (endpoint->state == ss::Endpoint::Connected))
		connect(sButton, SIGNAL(clicked()), this, SLOT(send()));
	else
		connect(sButton, SIGNAL(clicked()), this, SLOT(save()));

	grid->addWidget(label,      0, 0, 1, -1);

	grid->addWidget(traceLevelLabel,    1, 0, 1, 1);
	grid->addWidget(allTraceLevelsItem, 1, 2, 1, 2);
	grid->addWidget(traceLevelList,     3, 0, 5, 4);

	grid->addWidget(verboseBox,  9, 0);
	grid->addWidget(debugBox,   10, 0);
	grid->addWidget(readsBox,    9, 1);
	grid->addWidget(writesBox,  10, 1);
	grid->addWidget(toDoBox,     9, 2);
	grid->addWidget(sButton,    13, 0, 1, 4);

	memset(traceLevelItem, 0, sizeof(traceLevelItem));

	connect(allTraceLevelsItem, SIGNAL(clicked()), this, SLOT(all()));

	for (int ix = 0; ix < TRACE_LEVELS; ix++)
	{
		char* name;
		char  levelName[256];

		name = traceLevelName((TraceLevels) ix);
		if (name == NULL)
			continue;

		snprintf(levelName, sizeof(levelName), "%s (%d)", name, ix);
		traceLevelItem[ix] = new QListWidgetItem(levelName);
		traceLevelList->addItem(traceLevelItem[ix]);
		traceLevelItem[ix]->setCheckState(Qt::Checked);
	}

	// traceLevelList->adjustSize();
	traceLevelList->setFixedWidth(400);
	traceLevelList->setFixedHeight(500);
}



extern void workerUpdate(ss::Message::Worker* workerDataP);
/* ****************************************************************************
*
* ProcessConfigView::ProcessConfigView - 
*/
ProcessConfigView::ProcessConfigView(QGridLayout* grid, ss::Process* process, ss::Message::ConfigData* configData)
{
	// The data to fill this view is taken from:
	//
	// 1. The process itself, if it is running
	// 2. The Controller, if it is running
	// 3. Locally, from the Process structure
	//
	// When saving the data, it is saved locally in the Process structure and if possible, it is sent to the controller
	//

	init(grid, process);

	verboseBox->setCheckState((configData->verbose == true)? Qt::Checked : Qt::Unchecked);
	debugBox->setCheckState((configData->debug     == true)? Qt::Checked : Qt::Unchecked);
	readsBox->setCheckState((configData->reads     == true)? Qt::Checked : Qt::Unchecked);
	writesBox->setCheckState((configData->writes   == true)? Qt::Checked : Qt::Unchecked);
	toDoBox->setCheckState((configData->toDo       == true)? Qt::Checked : Qt::Unchecked);
						
	for (int ix = 0; ix < TRACE_LEVELS; ix++)
	{
	   if (traceLevelItem[ix])
		  traceLevelItem[ix]->setCheckState((configData->traceLevels[ix] == true)? Qt::Checked : Qt::Unchecked);
	}

	fill(grid, process);
}



/* ****************************************************************************
*
* ProcessConfigView::ProcessConfigView - 
*/
ProcessConfigView::ProcessConfigView(QGridLayout* grid, ss::Process* process, ss::Message::Worker* workerP)
{
	init(grid, process);

	workerUpdate(workerP);

	if ((process->host == NULL) || (process->host[0] == 0) || (strcmp(process->host, "ip") == 0))
		strncpy(process->host, workerP->ip, sizeof(process->host));

	verboseBox->setCheckState((workerP->verbose == true)? Qt::Checked : Qt::Unchecked);
	debugBox->setCheckState((workerP->debug     == true)? Qt::Checked : Qt::Unchecked);
	readsBox->setCheckState((workerP->reads     == true)? Qt::Checked : Qt::Unchecked);
	writesBox->setCheckState((workerP->writes   == true)? Qt::Checked : Qt::Unchecked);
	toDoBox->setCheckState((workerP->toDo       == true)? Qt::Checked : Qt::Unchecked);
						
	for (int ix = 0; ix < TRACE_LEVELS; ix++)
	{
		if (traceLevelItem[ix])
			traceLevelItem[ix]->setCheckState((workerP->traceV[ix] == true)? Qt::Checked : Qt::Unchecked);
	}

	fill(grid, process);
}



/* ****************************************************************************
*
* ProcessConfigView::ProcessConfigView - 
*/
ProcessConfigView::ProcessConfigView(QGridLayout* grid, ss::Process* process)
{
	// The data to fill this view is taken from:
	//
	// 1. The process itself, if it is running
	// 2. The Controller, if it is running
	// 3. Locally, from the Process structure
	//
	// When saving the data, it is saved locally in the Process structure and if possible, it is sent to the controller
	//

	init(grid, process);

	verboseBox->setCheckState((process->verbose == true)? Qt::Checked : Qt::Unchecked);
	debugBox->setCheckState((process->debug     == true)? Qt::Checked : Qt::Unchecked);
	readsBox->setCheckState((process->reads     == true)? Qt::Checked : Qt::Unchecked);
	writesBox->setCheckState((process->writes   == true)? Qt::Checked : Qt::Unchecked);
	toDoBox->setCheckState((process->toDo       == true)? Qt::Checked : Qt::Unchecked);
						
	for (int ix = 0; ix < TRACE_LEVELS; ix++)
	{
	   if (traceLevelItem[ix])
		  traceLevelItem[ix]->setCheckState((process->traceLevels[ix] == true)? Qt::Checked : Qt::Unchecked);
	}

	fill(grid, process);
}



/* ****************************************************************************
*
* fill - 
*/
void ProcessConfigView::fill(QGridLayout* grid, ss::Process* process)
{
#if 0
	verboseBox->setCheckState(Qt::Unchecked);
	debugBox->setCheckState(Qt::Unchecked);
	readsBox->setCheckState(Qt::Unchecked);
	writesBox->setCheckState(Qt::Unchecked);
	toDoBox->setCheckState(Qt::Unchecked);

	for (int ix = 0; ix < TRACE_LEVELS; ix++)
	{
		if (traceLevelItem[ix])
			traceLevelItem[ix]->setCheckState(Qt::Unchecked);
	}
#endif

	if (hostEditable == true)
	{
		QVBoxLayout* layout = tabManager->processListTab->righterLayout;
		ahLayout = new QVBoxLayout();

		hostLabel  = new QLabel("Host:");
		hostEdit   = new QLineEdit();
		hostEdit->setText(process->host);

		ahLayout->addSpacing(200);
		ahLayout->addWidget(hostLabel);		
		ahLayout->addWidget(hostEdit);
		ahLayout->addStretch(500);

		layout->addLayout(ahLayout);
	}
}



/* ****************************************************************************
*
* send - 
*/
void ProcessConfigView::send(void)
{
	ss::Message::ConfigData  configData;
	int                      s;
	char                     eText[256];

	memset(&configData, 0, sizeof(configData));

	configData.verbose    = (verboseBox->checkState() == Qt::Checked)? true : false;
	configData.debug      = (debugBox->checkState()   == Qt::Checked)? true : false;
	configData.reads      = (readsBox->checkState()   == Qt::Checked)? true : false;
	configData.writes     = (writesBox->checkState()  == Qt::Checked)? true : false;
	configData.toDo       = (toDoBox->checkState()    == Qt::Checked)? true : false;

	for (int ix = 0; ix < TRACE_LEVELS; ix++)
	{
		if (traceLevelItem[ix] != NULL)
			configData.traceLevels[ix] = (traceLevelItem[ix]->checkState() == Qt::Checked)? true : false;
		else
			configData.traceLevels[ix] = false;
	}

	s = iomMsgSend(process->endpoint, networkP->endpoint[0], ss::Message::ConfigSet, ss::Message::Ack, &configData, sizeof(configData));

	if (s == 0)
	{
		tabManager->processListTab->configShow(process->starterP);
		snprintf(eText, sizeof(eText), "New Log Configuration for process %s@%s sent.", process->name, process->host);
		new InfoWin("Config Info", eText, 1, 5);
	}
	else
	{
		snprintf(eText, sizeof(eText), "Error sending Log Configuration to process %s@%s.", process->name, process->host);
		new Popup("Message error", eText);
	}
}



/* ****************************************************************************
*
* all - 
*/
void ProcessConfigView::all(void)
{
	allTraceLevelsState = (allTraceLevelsState == true)? false : true;

	for (int ix = 0; ix < TRACE_LEVELS; ix++)
	{
		if (traceLevelItem[ix] != NULL)
			traceLevelItem[ix]->setCheckState((allTraceLevelsState == true)? Qt::Checked : Qt::Unchecked);
	}
}



/* ****************************************************************************
*
* processSave - 
*/
void ProcessConfigView::processSave(void)
{
	process->verbose    = (verboseBox->checkState() == Qt::Checked)? true : false;
	process->debug      = (debugBox->checkState()   == Qt::Checked)? true : false;
	process->reads      = (readsBox->checkState()   == Qt::Checked)? true : false;
	process->writes     = (writesBox->checkState()  == Qt::Checked)? true : false;
	process->toDo       = (toDoBox->checkState()    == Qt::Checked)? true : false;
	
	for (int ix = 0; ix < TRACE_LEVELS; ix++)
	{
		if (traceLevelItem[ix] != NULL)
			process->traceLevels[ix] = (traceLevelItem[ix]->checkState() == Qt::Checked)? true : false;
		else
			process->traceLevels[ix] = false;
	}

	if (hostEdit != NULL)
	{
		char* host;

		host = (char*) hostEdit->text().toStdString().c_str();
		strncpy(process->host, host, sizeof(process->host));
	}
}



/* ****************************************************************************
*
* save - 
*/
void ProcessConfigView::save(void)
{
	ss::Message::ConfigData  configData;
	int                      s;
	const char*              host = NULL;

	processSave();

	memset(&configData, 0, sizeof(configData));

	configData.verbose    = (verboseBox->checkState() == Qt::Checked)? true : false;
	configData.debug      = (debugBox->checkState()   == Qt::Checked)? true : false;
	configData.reads      = (readsBox->checkState()   == Qt::Checked)? true : false;
	configData.writes     = (writesBox->checkState()  == Qt::Checked)? true : false;
	configData.toDo       = (toDoBox->checkState()    == Qt::Checked)? true : false;

	for (int ix = 0; ix < TRACE_LEVELS; ix++)
	{
		if (traceLevelItem[ix] != NULL)
			configData.traceLevels[ix] = (traceLevelItem[ix]->checkState() == Qt::Checked)? true : false;
		else
			configData.traceLevels[ix] = false;
	}


	if (process->alias != NULL)
		strncpy(configData.alias, process->alias, sizeof(configData.alias));

	if (process->name != NULL)
		strncpy(configData.name, process->name, sizeof(configData.name));

	if (hostEdit != NULL)
	{
		host = hostEdit->text().toStdString().c_str();

		if ((host != NULL) && (host[0] != 0))
		{
			ss::Process* spawner = NULL;
			Host*        hostP;

			strncpy(configData.host, host, sizeof(configData.host));

			hostP = networkP->hostMgr->lookup(host);
			if (hostP != NULL)
				host = hostP->name;
			
			if ((host != NULL) && (processLookup(process->name, host) != NULL))
			{
				char eText[256];

				snprintf(eText, sizeof(eText), "There is already a process '%s' in host '%s'.\nCan't have two in the same machine ...", process->name, host);
				new Popup("Process already exists", eText);
				return;
			}

			networkP->hostMgr->insert(host, NULL);

			strncpy(process->host, host, sizeof(process->host));

			spawner = spawnerLookup(process->host);
			if (spawner == NULL)
			{
				int            fd;
				Starter*       starter;
				ss::Endpoint*  ep;

				fd = iomConnect(host, SPAWNER_PORT);
				if (fd == -1)
				{
					char eText[256];

					snprintf(eText, sizeof(eText),
							 "Unable to connect to samson platform in host '%s'.\n"
							 "This is a serious error.\n\n"
							 "Please make sure the samson platform processes run in '%s'\n"
							 "before you add a worker in this host.", host, host);

					new Popup("Cannot connect to Spawner", eText);

					LM_W(("Setting process->host to NULL ..."));
					process->host[0] = 0;
					return;
				}

				ep      = networkP->endpointAdd("Just connected to new spawner", fd, fd, "Spawner", "Spawner", 0, ss::Endpoint::Spawner, host, SPAWNER_PORT);
				spawner = spawnerAdd("Spawner", (char*) host, SPAWNER_PORT, ep);
				starter = starterAdd(spawner);

				if (starter == NULL)
					LM_X(1, ("NULL starter for Spawner@%s", host));
				
				if ((tabManager != NULL) && (tabManager->processListTab != NULL))
				{
					tabManager->processListTab->starterInclude(starter);
					starter->check("Just connected to new spawner");
				}
			}

			process->spawnerP = spawner;
		}

		process->starterP->nameButton->setText(QString(process->name) + "@" + process->host + " (" + process->alias + ")");
		process->starterP->startButton->setDisabled(false);
		process->starterP->logButton->setDisabled(true);
	}

	if ((networkP->endpoint[2] != NULL) && (networkP->endpoint[2]->state == ss::Endpoint::Connected))
	{
		s = iomMsgSend(networkP->endpoint[2], networkP->endpoint[0], ss::Message::ConfigChange, ss::Message::Msg, &configData, sizeof(configData));
		if (s != 0)
			LM_E(("iomMsgSend returned %d", s));
	}
	else
	{
		if (process->endpoint != networkP->endpoint[2])
			new Popup("Controller not connected", "Not connected to controller.\nCan only save this configuration information locally, sorry.\n");
	}

	tabManager->processListTab->configShow(process->starterP);
}
